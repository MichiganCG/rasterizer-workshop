/* This file is part of the Michigan Computer Graphics rasterization workshop.
 * Copyright (C) 2025  Aidan Rhys Donley
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "light.hpp"

#include <algorithm>
#include <iostream>

constexpr double epsilon = -1E-5;

Vec4 DirectionalLight::get_direction(const Vec4 &point) const
{
    std::ignore = point;
    return direction;
}

Vec4 PointLight::get_direction(const Vec4 &point) const
{
    return normalize(position - point);
}

float PointLight::get_attenuation(const Vec4 &point) const
{
    float distance_squared = magnitude_squared(position - point);
    return intensity / distance_squared;
}

Vec4 SpotLight::get_direction(const Vec4 &point) const
{
    return normalize(position - point);
}

float SpotLight::get_attenuation(const Vec4 &point) const
{
    float cos_angle = dot(get_direction(point), direction);
    if (cos_angle <= max_cos_angle) return 0;

    float light_fall_off = 1.0f - (1.0f - cos_angle) / (1.0f - max_cos_angle);
    return std::pow(light_fall_off, taper);
}

float saturate(float value) {
    return std::min(1.0f, std::max(0.0f, value));
}

Color Material::get_color(const Vec4 &world_coord, const Vec4 &normal, const Vec3 &texture_coord, const LightCollection &lights, const Camera &camera) const
{
    Color color, diffuse_sum, specular_sum;

    Vec4 N = normal;                  // normalized surface normal
    Vec4 V = normalize(camera.position - world_coord); // normalized vector pointing from the surface to the viewer

    // Compute the sum of the diffuse and specular light from each light source
    for (const auto &light : lights)
    {
        const Color &light_color = light->get_color();
        float attenuation = light->get_attenuation(world_coord);

        const Vec4 L = light->get_direction(world_coord); // normalized vector pointing from the surface to the light source
        float diffuse_intensity = saturate(dot(N, L));

#ifdef BLINN_PHONG_MODEL
        const Vec4 H = normalize(L + V);                  // normalized half vector between light and viewer directions
        float angle = saturate(dot(N, H));
#else
        const Vec4 R = normalize(2.0f * dot(L, N) * N - L); // normalized reflection vector
        float angle = saturate(dot(V, R));
#endif
        float specular_intensity = std::pow(angle, shininess);

        diffuse_sum  += light_color * attenuation * diffuse_intensity;
        specular_sum += light_color * attenuation * specular_intensity;
    }

    // Phong lighting model: sum of ambient, diffuse, and specular light
    color += ambient_color * lights.get_ambient_strength();
    color += diffuse_color * diffuse_sum;
    color += specular_color * specular_sum;

    // Use the texture's color if there is one
    if (texture_map) color *= texture_map.get_pixel(texture_coord.x, texture_coord.y);

    color.r = saturate(color.r);
    color.g = saturate(color.g);
    color.b = saturate(color.b);
    return color;
}

void Material::load_file(const std::string &file_name)
{
    std::fstream file;
    std::string line;

    file.open(file_name);

    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            std::stringstream ss(line);
            if (ss.eof())
                continue;

            std::string key;
            ss >> key;

            if (key == "#") // ignore comments
                continue;

            if (key == "Ns") // specular highlight focus
            {
                ss >> shininess;
                continue;
            }

            if (key == "Ka") // ambient color
            {
                float r, g, b;
                ss >> r >> g >> b;
                ambient_color = {r, g, b};
                continue;
            }

            if (key == "Kd") // diffuse color
            {
                float r, g, b;
                ss >> r >> g >> b;
                diffuse_color = {r, g, b};
                continue;
            }

            if (key == "Ks") // specular color
            {
                float r, g, b;
                ss >> r >> g >> b;
                specular_color = {r, g, b};
                continue;
            }

            if (key == "map_Kd") // texture image
            {
                std::string path;
                ss >> path;
                texture_map.load_file(path);
            }
        }
        file.close();
    }
    else
    {
        std::cerr << "Error: Unable to open file " << file_name << std::endl;
    }
}

void draw_line(Image &image, Vec3 &start, Vec3 &end)
{
    float u, v, du, dv, step;
    du = end.x - start.x;
    dv = end.y - start.y;

    if (std::abs(du) >= std::abs(dv))
        step = std::abs(du);
    else
        step = std::abs(dv);

    du = du / step;
    dv = dv / step;
    u = start.x;
    v = start.y;
    int i = 0;
    while (i <= step)
    {
        image.set_pixel(std::round(u), std::round(v), {1, 1, 1});
        u = u + du;
        v = v + dv;
        ++i;
    }
}

Vec3 calc_barycentric(Vec3 point, const Vec3 &s0, const Vec3 &s1, const Vec3 &s2)
{
    float area, a, b, c;

    // twice the triangle's area
    area = (s1.x - s0.x) * (s2.y - s0.y) - (s2.x - s0.x) * (s1.y - s0.y);

    // signed magnitude of the cross product (determinant)
    b = ((s0.x - s2.x) * (point.y - s2.y) - (point.x - s2.x) * (s0.y - s2.y)) / area;
    c = ((s1.x - s0.x) * (point.y - s0.y) - (point.x - s0.x) * (s1.y - s0.y)) / area;
    a = 1 - b - c;

    return Vec3(a, b, c);
}

void iterate_barycentric(Image &image, DepthBuffer &depth, const std::function<Color(float, float, float)> shader, const Vec3 &s0, const Vec3 &s1, const Vec3 &s2)
{
    // Get the bounding box of this triangle
    uint32_t minu = std::round(std::min({s0.x, s1.x, s2.x}));
    uint32_t maxu = std::round(std::max({s0.x, s1.x, s2.x}));
    uint32_t minv = std::round(std::min({s0.y, s1.y, s2.y}));
    uint32_t maxv = std::round(std::max({s0.y, s1.y, s2.y}));

    // Get the width and height of the bounding box
    uint32_t w = maxu - minu, h = maxv - minv;

    // Precompute values used to find the barycentric coordinates
    float area = (s1.x - s0.x) * (s2.y - s0.y) - (s2.x - s0.x) * (s1.y - s0.y);
    
    float z0 = s0.z, z1 = s1.z, z2 = s2.z; // get the depth of each vertex on the screen

    // Check the bounding box of the triangle
    auto action = [&](uint32_t i)
    {
        // Calculate the pixel coordinates
        uint32_t u = i % w + minu, v = i / w + minv;

        // Get the center of the pixel
        Vec3 pixel(u + 0.5f, v + 0.5f);

        // Compute the barycentric coordinates
        float b = ((s0.x - s2.x) * (pixel.y - s2.y) - (pixel.x - s2.x) * (s0.y - s2.y)) / area;
        float c = ((s1.x - s0.x) * (pixel.y - s0.y) - (pixel.x - s0.x) * (s1.y - s0.y)) / area;
        float a = 1 - b - c;

        // Check if this pixel is in the triangle
        if (!(a < epsilon || b < epsilon || c < epsilon))
        {
            float z = a * z0 + b * z1 + c * z2;
            // Check if this pixel is closer to the screen
            if (z < depth.at(u, v))
            {
                depth.at(u, v) = z;

                Color color = shader(a, b, c);
                image.set_pixel(u, v, color);
            }
        }
    };

    parallel_for(0, w * h, action, false);
}

void draw_barycentric(Image &image, DepthBuffer &depth, Color &color, Triplet triangle, VertexData &vertices)
{
    const VertexData::Vertex &v0 = vertices[triangle[0]], &v1 = vertices[triangle[1]], &v2 = vertices[triangle[2]];

    auto shader = [&](float a, float b, float c)
    {
        return color;
    };

    iterate_barycentric(image, depth, shader, v0.screen_coordinates, v1.screen_coordinates, v2.screen_coordinates);
}

void draw_barycentric(Image &image, DepthBuffer &depth, const Material &material, const LightCollection &lights, const Camera &camera, Triplet triangle, VertexData &vertices)
{
    const VertexData::Vertex &v0 = vertices[triangle[0]], &v1 = vertices[triangle[1]], &v2 = vertices[triangle[2]];
    float w0 = v0.clip_coordinates.w, w1 = v1.clip_coordinates.w, w2 = v2.clip_coordinates.w;

    auto shader = [&](float a, float b, float c)
    {
        // Correct for the perspective. https://www.cs.ucr.edu/~craigs/courses/2020-fall-cs-130/lectures/perspective-correct-interpolation.pdf
        float aw = a * w0, bw = b * w1, cw = c * w2;
        float w = 1.0f / (aw + bw + cw);

        // Interpolate across all of the vertex values
        Vec4 world =       w * (v0.world_coordinates   * aw + v1.world_coordinates   * bw + v2.world_coordinates   * cw);
        Vec4 normal = normalize(v0.world_normals       * aw + v1.world_normals       * bw + v2.world_normals       * cw);
        Vec3 texture =     w * (v0.texture_coordinates * aw + v1.texture_coordinates * bw + v2.texture_coordinates * cw);

        // Set the color using the material and lights
        return material.get_color(world, normal, texture, lights, camera);
    };

    iterate_barycentric(image, depth, shader, v0.screen_coordinates, v1.screen_coordinates, v2.screen_coordinates);
}
