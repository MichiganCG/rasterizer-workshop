#include "library.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"

#include <algorithm>
#include <atomic>
#include <iostream>
#include <random>
#include <stdexcept>
#include <thread>
#include <vector>

using Random = std::default_random_engine;
thread_local std::unique_ptr<Random> thread_random;

void Image::write_file(const std::string& path) const
{
	std::vector<uint8_t> data;
	data.reserve(width * height * 3);

	auto convert_single = [](float value)
	{
		//Gamma correction and clamp
		float corrected = std::sqrt(std::max(0.0f, std::min(value, 1.0f)));
		return static_cast<uint8_t>(corrected * std::numeric_limits<uint8_t>::max());
	};

	for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			auto& pixel = pixels[(height - y - 1) * width + x];
			data.push_back(convert_single(pixel.x));
			data.push_back(convert_single(pixel.y));
			data.push_back(convert_single(pixel.z));
		}
	}

	auto casted_width = static_cast<int>(width);
	auto casted_height = static_cast<int>(height);
	int result = stbi_write_png(path.c_str(), casted_width, casted_height, 3, data.data(), 0);
	if (result == 0) throw std::runtime_error("Error in STB library when outputting image.");
}

void Scene::insert_box(Vec3 center, Vec3 size, uint32_t material)
{
	Vec3 extend = size / 2.0f;
	boxes.emplace_back(center - extend, center + extend, material);
}

static float intersect_sphere(const Ray& ray, Vec3 center, float radius, Vec3& normal)
{
	Vec3 offset = ray.origin - center;
	float radius2 = radius * radius;
	float mapped = -dot(offset, ray.direction);

	float extend2 = mapped * mapped + radius2 - magnitude_squared(offset);
	if (extend2 < 0.0f) return Infinity;

	float extend = safe_sqrt(extend2);
	float distance = mapped - extend;
	if (distance < 0.0f) distance = mapped + extend;
	if (distance < 0.0f) return Infinity;

	normal = normalize(ray.direction * distance + offset);
	return distance;
}

static float intersect_plane(const Ray& ray, Vec3 normal, float offset)
{
	float mapped = dot(ray.direction, normal);

	if (not almost_zero(mapped))
	{
		float distance = dot(ray.origin, normal);
		distance = (distance + offset) / -mapped;
		if (distance >= 0.0f) return distance;
	}

	return Infinity;
}

static float intersect_box(const Ray& ray, Vec3 min, Vec3 max, Vec3& normal)
{
	Vec3 direction_r = Vec3(1.0f) / ray.direction;
	Vec3 lengths_min = (min - ray.origin) * direction_r;
	Vec3 lengths_max = (max - ray.origin) * direction_r;

	auto prepare = [](float& length_min, float& length_max, float& sign, float direction)
	{
		if (length_min < length_max) std::swap(length_min, length_max);
		sign = direction < 0.0f ? 1.0f : -1.0f;
	};

	Vec3 signs;
	prepare(lengths_min.x, lengths_max.x, signs.x, direction_r.x);
	prepare(lengths_min.y, lengths_max.y, signs.y, direction_r.y);
	prepare(lengths_min.z, lengths_max.z, signs.z, direction_r.z);

	float near = lengths_max.x;
	float far = lengths_min.x;
	Vec3 normal_near = Vec3(signs.x, 0.0f, 0.0f);
	Vec3 normal_far = Vec3(-signs.x, 0.0f, 0.0f);

	if (near < lengths_max.y)
	{
		near = lengths_max.y;
		normal_near = Vec3(0.0f, signs.y, 0.0f);
	}

	if (far > lengths_min.y)
	{
		far = lengths_min.y;
		normal_far = Vec3(0.0f, -signs.y, 0.0f);
	}

	if (near < lengths_max.z)
	{
		near = lengths_max.z;
		normal_near = Vec3(0.0f, 0.0f, signs.z);
	}

	if (far > lengths_min.z)
	{
		far = lengths_min.z;
		normal_far = Vec3(0.0f, 0.0f, -signs.z);
	}

	if ((far >= near) && (far >= 0.0f))
	{
		if (near >= 0.0f)
		{
			normal = normal_near;
			return near;
		}

		normal = normal_far;
		return far;
	}

	return Infinity;
}

bool Scene::intersect(const Ray& ray, float& distance, Vec3& normal, uint32_t& material) const
{
	distance = Infinity;

	for (auto& sphere : spheres)
	{
		Vec3 center = std::get<0>(sphere);
		float radius = std::get<1>(sphere);

		Vec3 new_normal;
		float new_distance = intersect_sphere(ray, center, radius, new_normal);

		if (new_distance < distance)
		{
			distance = new_distance;
			normal = new_normal;
			material = std::get<2>(sphere);
		}
	}

	for (auto& plane : planes)
	{
		Vec3 new_normal = std::get<0>(plane);
		float offset = std::get<1>(plane);
		float new_distance = intersect_plane(ray, new_normal, offset);

		if (new_distance < distance)
		{
			distance = new_distance;
			normal = new_normal;
			material = std::get<2>(plane);
		}
	}

	for (auto& box : boxes)
	{
		Vec3 min = std::get<0>(box);
		Vec3 max = std::get<1>(box);

		Vec3 new_normal;
		float new_distance = intersect_box(ray, min, max, new_normal);

		if (new_distance < distance)
		{
			distance = new_distance;
			normal = new_normal;
			material = std::get<2>(box);
		}
	}

	return std::isfinite(distance);
}

static Random* make_random_engine(uint32_t seed)
{
	auto random = std::make_unique<Random>(seed);
	Random* result = random.get();
	thread_random = std::move(random);
	return result;
}

float random_float()
{
	Random* random = thread_random.get();
	if (random == nullptr) random = make_random_engine(0);
	std::uniform_real_distribution<float> distribution;
	return distribution(*random);
}

static std::tuple<float, float> random_disk()
{
	float radius = safe_sqrt(random_float());
	float angle = Pi * 2.0f * random_float();
	return { std::cos(angle) * radius, std::sin(angle) * radius };
}

static std::tuple<Vec3, Vec3, Vec3> orthonormal_transform(Vec3 normal)
{
	Vec3 axis_x;
	Vec3 axis_y;

	if (almost_zero(normal.x) && almost_zero(normal.y))
	{
		axis_x = Vec3(1.0f, 0.0f, 0.0f);
		axis_y = normal.z > 0.0f ? Vec3(0.0f, 1.0f, 0.0f) : Vec3(0.0f, -1.0f, 0.0f);
	}
	else
	{
		axis_x = normalize(Vec3(normal.y, -normal.x, 0.0f));
		axis_y = cross(normal, axis_x);
	}

	return std::make_tuple(axis_x, axis_y, normal);
}

Vec3 random_cosine_hemisphere(Vec3 normal)
{
	auto [x, y] = random_disk();
	float z = safe_sqrt(1.0f - (x * x + y * y));
	Vec3 local(x, y, z);

	auto [axis_x, axis_y, axis_z] = orthonormal_transform(normal);
	return axis_x * local.x + axis_y * local.y + axis_z * local.z;
}

float fresnel_cos_i(float eta, float cos_o)
{
	float sin_o2 = 1.0f - cos_o * cos_o;
	float sin_i2 = eta * eta * sin_o2;
	if (sin_i2 >= 1.0f) return 0.0f; //Total internal reflection

	float cos_i = safe_sqrt(1.0f - sin_i2);
	return cos_o > 0.0f ? -cos_i : cos_i;
}

float fresnel_value(float eta, float cos_o, float cos_i)
{
	if (almost_zero(cos_i)) return 1.0f; //Total internal reflection

	cos_o = std::abs(cos_o);
	cos_i = std::abs(cos_i);

	float eta_r = eta;
	float para0 = cos_o * eta_r;
	float para1 = cos_i;
	float perp0 = cos_o;
	float perp1 = cos_i * eta_r;

	float para = (para0 - para1) / (para0 + para1);
	float perp = (perp0 - perp1) / (perp0 + perp1);
	return (para * para + perp * perp) / 2.0f;
}

Vec3 fresnel_refract(float eta, float cos_i, Vec3 outgoing, Vec3 normal)
{
	float cos_o = dot(outgoing, normal);
	return normalize(normal * (eta * cos_o + cos_i) - outgoing * eta);
}

void parallel_for(uint32_t begin, uint32_t end, const std::function<void(uint32_t)>& action, bool show_progress)
{
	auto print_done = []() { std::printf("\r       \rdone\n"); };

	if (end == begin)
	{
		if (show_progress) print_done();
		return;
	}

	if (end < begin) std::swap(begin, end);

	uint32_t workers = std::thread::hardware_concurrency();
	workers = std::min(std::max(workers, 1U), end - begin);

	std::vector<std::thread> threads;
	std::atomic<uint32_t> current = begin;

	for (uint32_t i = 0; i < workers; ++i)
	{
		auto entry = [i, end, &current, &action]()
		{
			make_random_engine(i);

			while (true)
			{
				uint32_t index = current++;
				if (index >= end) break;
				action(index);
			}
		};

		auto entry_print = [begin, end, &current, &action]()
		{
			make_random_engine(0);

			while (true)
			{
				uint32_t index = current++;
				if (index >= end) break;

				uint32_t done = index - begin;
				uint32_t total = end - begin;
				std::printf("\r%5.2f %%", static_cast<float>(done) / total * 100.0f);
				std::cout << std::flush;

				action(index);
			}
		};

		if (i > 0 || not show_progress) threads.emplace_back(entry);
		else threads.emplace_back(entry_print);
	}

	for (auto& thread : threads) thread.join();
	if (show_progress) print_done();
}

