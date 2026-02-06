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

#include <vector>
#include <memory>

#include "vectors.hpp"
#include "library.hpp"

class Light;

/**
 * A collection of lights in the scene.
 */
class LightCollection
{
public:
    LightCollection() : ambient_strength{0.01, 0.01, 0.01} {}
    LightCollection(const Color &ambient_strength) : ambient_strength{ambient_strength} {}

    const Color &get_ambient_strength() const { return ambient_strength; }

    void push_back(std::shared_ptr<const Light> light) { lights.push_back(light); }

    std::vector<std::shared_ptr<const Light>>::const_iterator begin() const { return lights.begin(); }
    std::vector<std::shared_ptr<const Light>>::const_iterator end() const { return lights.end(); }

private:
    // We store the ambient light seperately from the other lights since there should
    // only be a single source of ambient light.
    Color ambient_strength;

    // A vector of all lights in the scene.
    std::vector<std::shared_ptr<const Light>> lights;
};

class Light
{
public:
    Light(Color color) : color(color) {}
    virtual ~Light() = default;

    virtual Color get_color() const { return color; }
    virtual Vec4 get_direction(const Vec4 &point) const { return Vec4::ZERO; }
    virtual float get_attenuation(const Vec4 &point) const { return 1; }

private:
    Color color;
};

/**
 * A light source that is infinitely far away from the surface.
 * Assumes that the light rays are all parallel.
 */
class DirectionalLight : public Light
{
public:
    DirectionalLight(Color color, Vec4 direction) : Light(color), direction(-normalize(direction)) {}

    Vec4 get_direction(const Vec4 &point) const override;

private:
    // Stores the negative direction so that faces pointing towards the light
    // have a positive dot product.
    Vec4 direction;
};

/**
 * A light source at a specific point.
 */
class PointLight : public Light
{
public:
    PointLight(Color color, float intensity, Vec4 position) : Light(color), intensity(intensity), position(position) {}

    Vec4 get_direction(const Vec4 &point) const override;
    float get_attenuation(const Vec4 &point) const override;

private:
    float intensity;
    Vec4 position;
};

/**
 * A light source at a specific point, shining in the given direction and angle.
 */
class SpotLight : public Light
{
public:
    SpotLight(Color color, float angle, float taper, Vec4 direction, Vec4 position) : Light(color), max_cos_angle(std::cos(angle)), taper(taper), direction(-normalize(direction)), position(position) {}

    Vec4 get_direction(const Vec4 &point) const override;
    float get_attenuation(const Vec4 &point) const override;

private:
    float max_cos_angle, taper;
    Vec4 direction;
    Vec4 position;
};

/**
 * Basic material class using Wavefront .mtl files.
 */
class Material
{
public:
    Material() : shininess{0}, ambient_color{0}, diffuse_color{0}, specular_color{0} {}
    Material(const std::string &file_name) { load_file(file_name); }

    /**
     * Calculates the color at a point using the Blinn-Phong reflection model
     * https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model
     */
    Color get_color(const Vec4 &world_coord, const Vec4 &normal, const Vec3 &texture_coord, const LightCollection &lights, const Vec4 &camera) const;

private:
    void load_file(const std::string &file_name);

    float shininess;
    Color ambient_color, diffuse_color, specular_color;
    Image texture_map;
};
