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

#include "library/scene.hpp"

#include <string>
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Error: Must provide a scene config" << "\n";
        return 1;
    }
    std::string config = argv[1];

    SceneManager manager;
    Scene scene(config, manager);

    Image image(scene.get_width(), scene.get_height());
    DepthBuffer depth(scene.get_width(), scene.get_height());

    Matrix4 m_projection = perspective_projection(scene.get_fov(), scene.get_aspect_ratio(), 1, 100);
    Matrix4 m_screen = viewport(scene.get_width(), scene.get_height());

    for (const auto &object : scene.get_objects())
    {
        const Mesh &mesh = object->mesh;

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
                draw_barycentric(image, depth, object->material, scene.get_lights(), v0, v1, v2);
            }
        }
    }

    image.write_file("output.png");

    depth.get_image().write_file("depth.png");

    return 0;
}
