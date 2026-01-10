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

#include <fstream>

#include "../thirdparty/fkYAML/node.hpp"
#include "scene.hpp"

template <typename T>
const T &get_reference(const std::string &key, std::map<std::string, std::unique_ptr<T>> &unique_map) {
    if (!unique_map.contains(key))
        unique_map[key] = std::make_unique<T>(key);
    return *unique_map[key];
}

const Mesh &SceneManager::get_mesh(const std::string &name) { return get_reference(name, meshes); }

const Material &SceneManager::get_material(const std::string &name) { return get_reference(name, materials); }

void load_color(const fkyaml::node &root, Color &color)
{
    const auto &node = root["color"];
    color.r = node[0].get_value<float>();
    color.g = node[1].get_value<float>();
    color.b = node[2].get_value<float>();
}

void load_position(const fkyaml::node &root, Vec4 &position)
{
    const auto &node = root["position"];
    position.x = node[0].get_value<float>();
    position.y = node[1].get_value<float>();
    position.z = node[2].get_value<float>();
}

void load_direction(const fkyaml::node &root, Vec4 &direction)
{
    const auto &node = root["direction"];
    direction.x = node[0].get_value<float>();
    direction.y = node[1].get_value<float>();
    direction.z = node[2].get_value<float>();
    direction.w = 0.0f;
}

void load_rotation(const fkyaml::node &root, Quaternion &rotation)
{
    const auto &node = root["rotation"];
    Vec3 axis;
    axis.x = node[0].get_value<float>();
    axis.y = node[1].get_value<float>();
    axis.z = node[2].get_value<float>();
    float angle = node[3].get_value<float>();
    rotation = Quaternion{axis, angle};
}

void load_scale(const fkyaml::node &root, Vec3 &scale)
{
    const auto &node = root["scale"];
    scale.x = node[0].get_value<float>();
    scale.y = node[1].get_value<float>();
    scale.z = node[2].get_value<float>();
}

Scene::Scene(const std::string &config, SceneManager &manager)
{
    try
    {
        std::ifstream fs(config);

        fkyaml::node root = fkyaml::node::deserialize(fs);

        auto &resolution_node = root["resolution"];
        width = resolution_node["width"].get_value<uint32_t>();
        height = resolution_node["height"].get_value<uint32_t>();

        auto light_nodes = root["lights"];
        for (const auto& light_node : light_nodes)
        {
            std::string type = light_node["type"].as_str();

            Color color;
            load_color(light_node, color);

            std::shared_ptr<const Light> light;

            if (type == "directional")
            {
                Vec4 direction;
                load_direction(light_node, direction);

                light = std::make_shared<DirectionalLight>(color, direction);
            }
            else if (type == "point")
            {
                float intensity = light_node["intensity"].get_value<float>();

                Vec4 position;
                load_position(light_node, position);

                light = std::make_shared<PointLight>(color, intensity, position);
            }
            else if (type == "spot")
            {
                float angle = light_node["angle"].get_value<float>();
                float taper = light_node["taper"].get_value<float>();

                Vec4 direction;
                load_direction(light_node, direction);

                Vec4 position;
                load_position(light_node, position);

                light = std::make_shared<SpotLight>(color, angle, taper, direction, position);
            }

            lights.push_back(light);
        }

        auto object_nodes = root["objects"];
        for (const auto& object_node : object_nodes)
        {
            Vec4 position;
            Quaternion rotation;
            Vec3 scale;

            load_position(object_node, position);
            load_rotation(object_node, rotation);
            load_scale(object_node, scale);

            auto object = std::make_shared<Object>(
                position,
                rotation,
                scale,
                manager.get_mesh(object_node["mesh"].as_str()),
                manager.get_material(object_node["material"].as_str())
            );
            objects.push_back(object);
        }
    }
    catch (const fkyaml::exception &e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
