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

#include <fstream>
#include <iostream>

float saturate(float value) {
    return std::min(1.0f, std::max(0.0f, value));
}

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

Color Material::get_color(const Vec4 &world_coord, const Vec4 &normal, const Vec3 &texture_coord, const LightCollection &lights, const Vec4 &camera) const
{
    Color color, diffuse_sum, specular_sum;

    Vec4 N = normal;                          // normalized surface normal
    Vec4 V = normalize(camera - world_coord); // normalized vector pointing from the surface to the viewer

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
