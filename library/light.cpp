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

#include <iostream>

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
    return 1.0f / magnitude_squared(position - point) * k;
}

Vec4 SpotLight::get_direction(const Vec4 &point) const
{
    return normalize(position - point);
}

float SpotLight::get_attenuation(const Vec4 &point) const
{
    float light_fall_off = std::max(0.0f, dot(normalize(point - position), direction) - angle) / (1.0f - angle);
    return std::pow(light_fall_off, taper);
}

Color Material::get_color(const Vec4 &point, const Vec4 &normal, const Vec3 &uv, LightCollection &lights)
{
    Color diffuse_sum, specular_sum;

    // Compute the sum of the diffuse and specular light from each light source
    for (const Light *light : lights)
    {
        const Color &color = light->get_color();
        const float attenuation = light->get_attenuation(point);
        // https://web.stanford.edu/class/ee267/lectures/lecture3.pdf
        const Vec4 L = light->get_direction(point); // normalized vector pointing twoards the light source
        const Vec4 &N = normal;                            // normalized surface normal
        const Vec4 V = normalize(point);            // normalized vector pointing towards the viewer
        const Vec4 R = L - N * dot(N, L) * 2;       // normalized reflection on surface normal

        diffuse_sum += color * std::max(0.0f, dot(N, L)) * attenuation;
        specular_sum += color * std::pow(std::max(0.0f, dot(R, V)), shininess) * attenuation;
    }

    // Use the texture's color if there is one
    Color diffuse_color = texture_map ? texture_map.get_pixel(uv.x, uv.y) : diffuse;
    Color specular_color = roughness_map ? roughness_map.get_pixel(uv.x, uv.y) : specular;

    Color color;
    // Phong lighting model: Sum of ambient, diffuse, and specular light
    color = ambient * lights.get_ambient() + diffuse_color * diffuse_sum + specular_color * specular_sum;
    color.r = std::min(1.0f, color.r);
    color.g = std::min(1.0f, color.g);
    color.b = std::min(1.0f, color.b);
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
                ambient = {r, g, b};
                continue;
            }

            if (key == "Kd") // diffuse color
            {
                float r, g, b;
                ss >> r >> g >> b;
                diffuse = {r, g, b};
                continue;
            }

            if (key == "Ks") // specular color
            {
                float r, g, b;
                ss >> r >> g >> b;
                specular = {r, g, b};
                continue;
            }

            if (key == "map_Kd") // texture image
            {
                std::string path;
                ss >> path;
                texture_map.load_file(path);
            }

            if (key == "map_Nm") // normal map
            {
                std::string path;
                ss >> path;
                normal_map.load_file(path);
            }
            
            if (key == "map_Ks") // roughness map
            {
                std::string path;
                ss >> path;
                roughness_map.load_file(path);
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

void iterate_barycentric(uint32_t width, uint32_t height, std::function<void(uint32_t, uint32_t, float, float, float)> func, const Vec3 &v0, const Vec3 &v1, const Vec3 &v2)
{
    uint32_t minu = std::clamp(std::min({v0.x, v1.x, v2.x}), 0.0f, static_cast<float>(width));
    uint32_t maxu = std::clamp(std::max({v0.x, v1.x, v2.x}), 0.0f, static_cast<float>(width));
    uint32_t minv = std::clamp(std::min({v0.y, v1.y, v2.y}), 0.0f, static_cast<float>(height));
    uint32_t maxv = std::clamp(std::max({v0.y, v1.y, v2.y}), 0.0f, static_cast<float>(height));

    // Precompute values used to find the barycentric coordinates
    Vec3 edge0 = v1 - v0, edge1 = v2 - v0;
    float d00 = dot(edge0, edge0);
    float d01 = dot(edge0, edge1);
    float d11 = dot(edge1, edge1);
    float area = d00 * d11 - d01 * d01;

    // Check the bounding box of the triangle
    for (uint32_t u = minu; u <= maxu; ++u)
    {
        for (uint32_t v = minv; v <= maxv; ++v)
        {
            // Get the center of the pixel
            Vec3 pixel(u + 0.5f, v + 0.5f);

            // Compute the barycentric coordinates
            Vec3 edge2 = pixel - v0;
            float d20 = dot(edge2, edge0);
            float d21 = dot(edge2, edge1);

            float b = (d11 * d20 - d01 * d21) / area;
            float c = (d00 * d21 - d01 * d20) / area;
            float a = 1.0f - b - c;

            // Check if this pixel is in the triangle
            if (a >= 0 && b >= 0 && c >= 0)
            {
                func(u, v, a, b, c);
            }
        }
    }
}

void draw_barycentric(Image &image, DepthBuffer &depth, Color &color, const Vertex &v0, const Vertex &v1, const Vertex &v2)
{
    float z0 = 1.0f / v0.screen.z, z1 = 1.0f / v1.screen.z, z2 = 1.0f / v2.screen.z;

    auto draw = [&](uint32_t u, uint32_t v, float a, float b, float c)
    {
        float z = 1 / (a * z0 + b * z1 + c * z2);
        // Check if this pixel is closer to the screen
        if (z <= depth.at(u, v))
        {
            depth.at(u, v) = z;

            image.set_pixel(u, v, color);
        }
    };

    iterate_barycentric(image.get_width(), image.get_height(), draw, v0.screen, v1.screen, v2.screen);
}

void draw_barycentric(Image &image, DepthBuffer &depth, Material &material, LightCollection &lights, const Vertex &v0, const Vertex &v1, const Vertex &v2)
{
    float z0 = 1.0f / v0.screen.z, z1 = 1.0f / v1.screen.z, z2 = 1.0f / v2.screen.z;
    float w0 = v0.clip.w, w1 = v1.clip.w, w2 = v2.clip.w;

    auto draw = [&](uint32_t u, uint32_t v, float a, float b, float c)
    {
        float z = 1 / (a * z0 + b * z1 + c * z2);
        // Check if this pixel is closer to the screen
        if (z <= depth.at(u, v))
        {
            depth.at(u, v) = z;

            /*
             * Correct for the perspective.
             * https://www.cs.ucr.edu/~craigs/courses/2020-fall-cs-130/lectures/perspective-correct-interpolation.pdf
             */
            float aw = a * w0, bw = b * w1, cw = c * w2;
            float w = 1 / (aw + bw + cw);

            // Interpolate across all of the vertex values
            Vec4 world = (v0.world * aw + v1.world * bw + v2.world * cw) * w;
            Vec3 texture = (v0.texture * aw + v1.texture * bw + v2.texture * cw) * w;
            Vec4 normal = normalize(v0.normal * aw + v1.normal * bw + v2.normal * cw);

            // Set the color using the material and light
            Color color = material.get_color(world, normal, texture, lights);
            image.set_pixel(u, v, color);
        }
    };

    iterate_barycentric(image.get_width(), image.get_height(), draw, v0.screen, v1.screen, v2.screen);
}
