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

#include "library/vectors.hpp"
#include "library/quaternion.hpp"
#include "library/matrix.hpp"
#include "library/mesh.hpp"
#include "library/light.hpp"
#include "library/library.hpp"

#include <string>
#include <iostream>

const uint32_t ImageWidth = 960;
const uint32_t ImageHeight = 540;
const float AspectRatio = (float)ImageWidth / (float)ImageHeight;

struct Object
{
    Vec4 position;
    Quaternion rotation;
    Vec3 scale;
    Mesh &mesh;
    Material &material;
};

int main()
{
    Image image(ImageWidth, ImageHeight);
    DepthBuffer depth(ImageWidth, ImageHeight);

    Matrix4 m_projection = perspective_projection(70, AspectRatio, 1, 100);
    Matrix4 m_screen = viewport(ImageWidth, ImageHeight);

    // Load models
    Mesh cube("model/cube.obj");
    Mesh sphere("model/uv_sphere.obj");
    Mesh plane("model/plane.obj");

    // Load materials
    Material material("material/material.mtl");
    Material tile("material/tile.mtl");

    DirectionalLight sky_light({1.0, 0.941, 0.91}, {-1, -1, -1, 0});
    PointLight red_light({1, 0, 0}, 2, {0, 0.5, -4.5});
    SpotLight green_light({0.6, 0.8, 0.5}, 0.3, 1, {1, -0.5, -1, 0}, {-5, 3, 0});
    PointLight blue_light({0, 0, 1}, 5, {2, 0.75, -3});

    // Set lights
    LightCollection lights;
    lights.push_back(&sky_light);
    lights.push_back(&red_light);
    lights.push_back(&green_light);
    lights.push_back(&blue_light);

    Object ground({0, -1, -50}, {}, {150, 1, 100}, plane, material);
    Object tile_cube({1.2, 0, -5}, {{0, 1, 0}, Pi/3}, {0.8}, cube, tile);
    Object white_sphere({-1.2, 0, -5}, {}, {1}, sphere, material);

    // Set objects
    std::vector<Object *> objects;
    objects.push_back(&ground);
    objects.push_back(&tile_cube);
    objects.push_back(&white_sphere);

    for (Object *object : objects)
    {
        Mesh &mesh = object->mesh;

        // Define the model matrix
        Matrix4 m_model = translate(object->position) * rotate(object->rotation) * scale(object->scale);

        std::vector<Vec4> world_vertices(mesh.vertex_size());
        std::vector<Vec4> world_normals(mesh.vertex_size());

        std::vector<Vec4> clip_vertices(mesh.vertex_size());

        // Transform all vertices in the mesh to world space and then to clip space
        for (size_t i = 0; i < mesh.vertex_size(); ++i)
        {
            world_vertices[i] = m_model * mesh.get_vertex(i);
            world_normals[i] = m_model * mesh.get_normal(i);

            clip_vertices[i] = m_projection * world_vertices[i];
        }

        // Loop through all triangles in the mesh
        for (size_t i = 0; i < mesh.size(); ++i)
        {
            Triangle triangle = mesh[i];
            std::vector<Vertex> vertices(3);

            for (size_t t = 0; t < 3; ++t)
            {
                vertices[t].world = world_vertices[triangle[t]];
                vertices[t].clip = clip_vertices[triangle[t]];
                vertices[t].normal = world_normals[triangle[t]];
                vertices[t].texture = mesh.get_texture(triangle[t]);
            }

            // Backface culling
            Vec4 view_vector = -normalize(vertices[0].world);
            Vec4 normal_vector = cross(vertices[1].world - vertices[0].world, vertices[2].world - vertices[0].world);
            if (dot(view_vector, normal_vector) < 0)
                continue;

            // Clip triangles such that they are bounded within [-w, w] on all axes
            sutherland_hodgman(vertices);

            if (vertices.size() == 0)
                continue;

            // Transform from clip space to screen space
            for (size_t j = 0; j < vertices.size(); ++j)
            {
                Vertex &vertex = vertices[j];
                Vec4 &clip = vertex.clip;

                float temp = clip.w;
                if (temp != 0) {
                    temp = 1.0f / temp;
                    clip *= temp;
                }

                vertex.screen = m_screen * clip;

                clip.w = temp; // store the w value for later
            }

            // Reform triangles using fan triangulation
            for (size_t j = 1; j < vertices.size() - 1; ++j)
            {
                Vertex &v0 = vertices[0], &v1 = vertices[j], &v2 = vertices[j + 1];

                // Draw each triangle
                draw_barycentric(image, depth, object->material, lights, v0, v1, v2);
            }
        }
    }
    image.write_file("output.png");

    depth.get_image().write_file("depth.png");

    return 0;
}
