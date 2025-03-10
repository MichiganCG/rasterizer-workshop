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

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "vectors.hpp"
#include "mesh.hpp"
#include "library.hpp"

class Light;

/**
 * A collection of lights in the scene.
 */
class LightCollection
{
    Color ambient;
    std::vector<Light *> lights;

public:
    LightCollection() : ambient{0.01, 0.01, 0.01} {}
    LightCollection(const Color &ambient) : ambient{ambient} {}

    const Color &get_ambient() { return ambient; }

    void push_back(Light *light) { lights.push_back(light); }

    std::vector<Light *>::iterator begin() { return lights.begin(); }
    std::vector<Light *>::iterator end() { return lights.end(); }
};

class Light
{
    Color color;

public:
    Light(LightCollection &lights, Color color) : color(color) { lights.push_back(this); }
    virtual ~Light() = default;

    virtual const Color &get_color() const { return color; }
    virtual Vec4 get_direction(const Vec4 &point) const { return Vec4::ZERO; }
    virtual float get_attenuation(const Vec4 &point) const { return 1; }
};

/**
 * A light source that is infinitely far away from the surface.
 * Assumes that the light rays are all parallel.
 */
class DirectionalLight : public Light
{
    Vec4 direction;

public:
    DirectionalLight(LightCollection &lights, Color color, Vec4 direction) : Light(lights, color), direction(-normalize(direction)) {}

    Vec4 get_direction(const Vec4 &point) const override;
};

/**
 * A light source at a specific point.
 */
class PointLight : public Light
{
    float k;
    Vec4 position;

public:
    PointLight(LightCollection &lights, Color color, float k, Vec4 position) : Light(lights, color), k(k), position(position) {}

    Vec4 get_direction(const Vec4 &point) const override;
    float get_attenuation(const Vec4 &point) const override;
};

/**
 * A light source at a specific point, shining in the given direction and angle.
 */
class SpotLight : public Light
{
    float angle, taper;
    Vec4 direction;
    Vec4 position;

public:
    SpotLight(LightCollection &lights, Color color, float angle, float taper, Vec4 direction, Vec4 position) : Light(lights, color), angle(std::cos(angle)), taper(taper), direction(normalize(direction)), position(position) {}

    Vec4 get_direction(const Vec4 &point) const override;
    float get_attenuation(const Vec4 &point) const override;
};

/**
 * Basic material class using Wavefront .mtl files.
 */
class Material
{
    float shininess;
    Color ambient, diffuse, specular;
    Image texture_map, roughness_map, normal_map;

public:
    Material() : shininess(0), ambient{0}, diffuse{0}, specular{0} {}
    Material(const std::string &file_name) { load_file(file_name); }

    void load_file(const std::string &file_name);

    Color get_color(const Vec4 &point, const Vec4 &normal, const Vec3 &uv, LightCollection &lights);
};

/**
 * Uses the Digital Differential Analyzer (DDA) method to draw a line from 'start' to 'end'.
 */
void draw_line(Image &image, const Vec3 &start, const Vec3 &end);

/**
 * Uses barycentric coordinates to fill a triangle.
 */
void draw_barycentric(Image &image, DepthBuffer &depth, Color &color, const Vertex &v0, const Vertex &v1, const Vertex &v2);

/**
 * Uses barycentric coordinates to fill a triangle.
 * Uses the material and light sources provided to determine color.
 */
void draw_barycentric(Image &image, DepthBuffer &depth, Material &mat, LightCollection &lights, const Vertex &v0, const Vertex &v1, const Vertex &v2);
