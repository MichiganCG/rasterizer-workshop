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
#include <string>
#include <vector>
#include <memory>
#include <map>

#include "vectors.hpp"
#include "quaternion.hpp"
#include "mesh.hpp"
#include "light.hpp"

/**
 * Manages the meshes and textures used by objects across scenes.
 * 
 * Stores only a single copy of each mesh and material to reduce memory cost of duplicate objects.
 */
class SceneManager {
public:
    const Mesh &get_mesh(const std::string &name);
    const Material &get_material(const std::string &name);

private:
    std::map<std::string, std::unique_ptr<const Mesh>> meshes;
    std::map<std::string, std::unique_ptr<const Material>> materials;
};

class Object
{
public:
    Vec4 position;
    Quaternion rotation;
    Vec3 scale;
    const Mesh &mesh;
    const Material &material;
};

class Scene {
public:
    Scene(const std::string &config, SceneManager &manager);

    const std::vector<std::shared_ptr<Object>> &get_objects() const { return objects; }
    const LightCollection &get_lights() const { return lights; }

    uint32_t get_width() const { return width; }
    uint32_t get_height() const { return height; }
    float get_aspect_ratio() const { return (float)width / (float)height; }

private:
    uint32_t width, height;

    std::vector<std::shared_ptr<Object>> objects;
    LightCollection lights;
};
