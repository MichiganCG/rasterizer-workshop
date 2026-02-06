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

#include "scene.hpp"

#include "../thirdparty/fkYAML/node.hpp"

#include <fstream>
#include <iostream>

template <typename T>
const T &get_reference(const std::string &key, std::map<std::string, std::unique_ptr<T>> &unique_map)
{
    if (!unique_map.contains(key))
        unique_map[key] = std::make_unique<T>(key);
    return *unique_map[key];
}

const Mesh &SceneManager::get_mesh(const std::string &name) { return get_reference(name, meshes); }

const Material &SceneManager::get_material(const std::string &name) { return get_reference(name, materials); }

void from_node(const fkyaml::node &node, Color &color)
{
    switch (node.size())
    {
        case 0:
            break;
        case 3:
            color.r = node[0].get_value<float>();
            color.g = node[1].get_value<float>();
            color.b = node[2].get_value<float>();
            break;
        default:
            throw fkyaml::exception("Invalid color format");
    }
}

void from_node(const fkyaml::node &node, Vec4 &vector)
{
    switch (node.size())
    {
        case 0:
            break;
        case 3:
            vector.x = node[0].get_value<float>();
            vector.y = node[1].get_value<float>();
            vector.z = node[2].get_value<float>();
            break;
        default:
            throw fkyaml::exception("Invalid vector format");
    }
}

void from_node(const fkyaml::node &node, Quaternion &rotation)
{
    switch (node.size())
    {
        case 0:
            break;
        case 4:
        {
            Vec3 axis;
            axis.x = node[0].get_value<float>();
            axis.y = node[1].get_value<float>();
            axis.z = node[2].get_value<float>();
            float angle = node[3].get_value<float>();
            rotation.rotate(axis, angle);
            break;
        }
        default:
            throw fkyaml::exception("Invalid quaternion format");
    }
}

Scene::Scene(const std::string &config, SceneManager &manager)
    : width(400), height(300), fov(70)
{
    try
    {
        std::ifstream fs(config);

        fkyaml::node root = fkyaml::node::deserialize(fs);

        width = root["resolution"]["width"].get_value<uint32_t>();
        height = root["resolution"]["height"].get_value<uint32_t>();
        fov = root["fov"].get_value<float>();

        if (root.contains("camera"))
        {
            camera.position = root["camera"]["position"].get_value<Vec4>();
            camera.rotation = root["camera"]["rotation"].get_value<Quaternion>();
        }

        if (root.contains("lights") && root["lights"].is_sequence())
        {
            for (const auto &light_node : root["lights"])
            {
                const std::string &type = light_node["type"].as_str();

                Color color = light_node["color"].get_value<Color>();

                std::shared_ptr<const Light> light;

                if (type == "directional")
                {
                    Vec4 direction = light_node["direction"].get_value<Vec4>();
                    direction.w = 0.0f;
                    light = std::make_shared<DirectionalLight>(color, direction);
                }
                else if (type == "point")
                {
                    float intensity = light_node["intensity"].get_value<float>();
                    Vec4 position = light_node["position"].get_value<Vec4>();
                    light = std::make_shared<PointLight>(color, intensity, position);
                }
                else if (type == "spot")
                {
                    float angle = light_node["angle"].get_value<float>();
                    float taper = light_node["taper"].get_value<float>();
                    Vec4 direction = light_node["direction"].get_value<Vec4>();
                    direction.w = 0.0f;
                    Vec4 position = light_node["position"].get_value<Vec4>();
                    light = std::make_shared<SpotLight>(color, angle, taper, direction, position);
                }

                lights.push_back(light);
            }
        }

        if (root.contains("objects") && root["objects"].is_sequence()) {
            for (const auto &object_node : root["objects"])
            {
                auto object = std::make_shared<Object>(
                    object_node["position"].get_value<Vec4>(),
                    object_node["rotation"].get_value<Quaternion>(),
                    object_node["scale"].get_value<Vec4>(),
                    manager.get_mesh(object_node["mesh"].as_str()),
                    manager.get_material(object_node["material"].as_str())
                );

                objects.push_back(object);
            }
        }
    }
    catch (const fkyaml::exception &e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
