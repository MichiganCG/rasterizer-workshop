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

    Timer timer;

    for (const auto &object : scene.get_objects())
    {
        const Mesh &mesh = object->mesh;

        // Define the model matrix
        Matrix4 m_model = translate(object->position) * rotate(object->rotation) * scale(object->scale);

        VertexData vertices{mesh.vertex_size()};

        // Transform all vertices in the mesh to world space and then to clip space
        for (size_t i = 0; i < mesh.vertex_size(); ++i)
        {
            vertices[i].world_coordinates   = m_model * mesh.get_vertex(i);
            vertices[i].world_normals       = m_model * mesh.get_normal(i);
            vertices[i].clip_coordinates    = m_projection * vertices[i].world_coordinates;
            vertices[i].texture_coordinates = mesh.get_texture(i);
        }

        std::vector<Triplet> triangles;

        // Loop through all triangles in the mesh
        for (size_t i = 0; i < mesh.size(); ++i)
        {
            Triplet triangle = mesh[i];
            std::vector<uint32_t> indices{triangle[0], triangle[1], triangle[2]};

            // Clip triangles such that they are bounded within [-w, w] on all axes
            sutherland_hodgman(indices, vertices);

            if (indices.size() == 0)
                continue;

            // Reform triangles using fan triangulation
            for (size_t j = 1; j < indices.size() - 1; ++j)
            {
                triangles.emplace_back(indices[0], indices[j], indices[j + 1]);
            }
        }

        // Transform from clip space to screen space
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            Vec4 &clip = vertices[i].clip_coordinates;

            // Scale by the depth
            float temp = clip.w;
            if (temp != 0) {
                temp = 1.0f / temp;
                clip *= temp;
            }

            vertices[i].screen_coordinates = m_screen * clip;

            clip.w = temp; // store the w value for later
        }

        // Draw each triangle
        for (size_t i = 0; i < triangles.size(); ++i)
        {
            Triplet triangle = triangles[i];

            // Backface culling
            Vec4 ab = vertices[triangle[1]].clip_coordinates - vertices[triangle[0]].clip_coordinates;
            Vec4 ac = vertices[triangle[2]].clip_coordinates - vertices[triangle[0]].clip_coordinates;

            // Ignore triangles that are ordered incorrectly
            float orientation = ab.x * ac.y - ac.x * ab.y;
            if (orientation < 0.0f)
                continue;

            // Draw each triangle
            draw_barycentric(image, depth, object->material, scene.get_lights(), triangle, vertices);
        }
    }

    std::cout << timer.elapsed() << " milliseconds\n";

    image.write_file("output.png");
    depth.get_image().write_file("depth.png");

    return 0;
}
