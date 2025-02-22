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
			data.push_back(convert_single(pixel.r));
			data.push_back(convert_single(pixel.g));
			data.push_back(convert_single(pixel.b));
		}
	}

	auto casted_width = static_cast<int>(width);
	auto casted_height = static_cast<int>(height);
	int result = stbi_write_png(path.c_str(), casted_width, casted_height, 3, data.data(), 0);
	if (result == 0) throw std::runtime_error("Error in STB library when outputting image.");
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

