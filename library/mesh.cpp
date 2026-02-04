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

#include "mesh.hpp"

#include <algorithm>
#include <optional>
#include <iostream>

std::ostream &operator<<(std::ostream &os, const Triplet &rhs)
{
    os << "( " << rhs[0] << " " << rhs[1] << " " << rhs[2] << " )";
    return os;
}

// https://stackoverflow.com/a/9237226
void remove_duplicates(std::vector<std::string> &arr)
{
    std::sort(arr.begin(), arr.end());
    arr.erase(std::unique(arr.begin(), arr.end()), arr.end());
}

void Mesh::smooth_normals()
{
    for (size_t i = 0; i < normals.size(); ++i)
        normals[i].w = 0;

    for (size_t i = 0; i < size(); ++i)
    {
        Triplet tri = at(i);
        // Compute the normal of each face and add it to the normal of each vertex

        Vec4 edge1 = vertices[tri[1]] - vertices[tri[0]];
        Vec4 edge2 = vertices[tri[2]] - vertices[tri[0]];
        Vec4 normal = cross(edge1, edge2);

        normals[tri[0]] += normal;
        normals[tri[1]] += normal;
        normals[tri[2]] += normal;
    }

    for (size_t i = 0; i < normals.size(); ++i)
        normals[i] = normalize(normals[i]);
}

void Mesh::load_file(const std::string &file_name)
{
    count = 0;

    std::vector<Vec4> cached_vertices;
    std::vector<Vec3> cached_textures;
    std::vector<Vec4> cached_normals;

    std::vector<std::string> faces;
    std::vector<std::string> corrected_index_mapping;

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

            if (key == "v") // geometric vertices
            {
                float x, y, z;
                ss >> x >> y >> z;
                cached_vertices.push_back({x, y, z, 1});
                continue;
            }

            if (key == "vt") // texture coordinates
            {
                float u, v;
                ss >> u >> v;
                cached_textures.push_back({u, v});
                continue;
            }

            if (key == "vn") // vertex normals
            {
                float x, y, z;
                ss >> x >> y >> z;
                cached_normals.push_back({x, y, z, 0});
                continue;
            }

            if (key == "f") // face element
            {
                // Assumes that each face has at least 3 vertices
                size_t temp = 0;
                std::string face;
                std::getline(ss, face);

                // push back the whole line for later processing
                faces.push_back(face);

                // push back each index set for re-indexing
                std::stringstream fss(face);
                while (!fss.eof())
                {
                    std::string index;
                    fss >> index;
                    corrected_index_mapping.push_back(index);
                    // count the number of faces
                    ++temp;
                }
                if (temp < 3)
                    throw std::runtime_error("Error when reading .obj.");

                count += temp - 2;
                continue;
            }
        }
        file.close();

        /**
         * Wavefront .obj files allow for more complexity than vertex arrays.
         * Each attribute (vertex, texture, normal) can have its own index.
         * In our vertex array, we want each element (a shared vertex, texture,
         * and normal) to have the same index.
         *
         * 1. Remove any duplicate elements. This gives us a vector
         *    where the element's index in the vector is its new index.
         * 2. Add all of the unique elements to the mesh in order.
         * 3. Add triplets of indices to the vertex array using the new indices.
         */
        remove_duplicates(corrected_index_mapping);
        size_t index_count = corrected_index_mapping.size();

        bool has_normals = !normals.empty();

        vertices.resize(index_count);
        textures.resize(index_count);
        normals.resize(index_count);

        for (size_t i = 0; i < index_count; ++i)
        {
            // Cases: v, v/t, v//n, v/t/n
            std::stringstream ess(corrected_index_mapping[i]);

            int vertex_index;
            ess >> vertex_index;
            vertices[i] = cached_vertices[vertex_index - 1];

            if (!ess.eof() && ess.peek() == '/')
            {
                // Cases: v/t, v//n, v/t/n
                char slash;
                ess >> slash;

                if (!ess.eof() && ess.peek() != '/')
                {
                    // Cases: v/t, v/t/n
                    int texture_index;
                    ess >> texture_index;
                    textures[i] = cached_textures[texture_index - 1];
                }

                if (!ess.eof() && ess.peek() == '/')
                {
                    // Cases: v//n, v/t/n
                    int normal_index;
                    ess >> slash >> normal_index;
                    normals[i] = cached_normals[normal_index - 1];
                }
            }
        }

        elements.resize(count * 3);
        size_t offset = 0;

        for (auto &face : faces)
        {
            std::stringstream ss(face);
            std::vector<std::string> face_elements;

            while (!ss.eof())
            {
                std::string element;
                ss >> element;
                face_elements.push_back(element);
            }

            std::vector<int> indices;
            indices.resize(face_elements.size());

            for (size_t i = 0; i < face_elements.size(); ++i)
            {
                auto it = std::find(corrected_index_mapping.begin(), corrected_index_mapping.end(), face_elements[i]);
                if (it == corrected_index_mapping.end())
                    std::cerr << "Error: Mismatch element in " << file_name << std::endl;
                int index = std::distance(corrected_index_mapping.begin(), it);

                indices[i] = index;
            }

            for (size_t i = 1; i < face_elements.size() - 1; ++i)
            {
                elements[offset + 0] = indices[0];
                elements[offset + 1] = indices[i];
                elements[offset + 2] = indices[i + 1];
                offset += 3;
            }
        }

        if (!has_normals)
            smooth_normals();
    }
    else
    {
        std::cerr << "Error: Unable to open file " << file_name << std::endl;
    }
}

const std::vector<Vec4> clipping_planes = {
    {-1, 0, 0}, // left
    {1, 0, 0},  // right
    {0, -1, 0}, // bottom
    {0, 1, 0},  // top
    {0, 0, -1}, // near
    {0, 0, 1},  // far
};

void sutherland_hodgman(std::vector<uint32_t> &input_list, VertexData &vertices)
{
    std::vector<uint32_t> out_list = input_list;

    for (const Vec4 &plane : clipping_planes)
    {
        std::swap(input_list, out_list);
        out_list.clear();

        if (input_list.size() == 0)
            return;

        uint32_t start = input_list[input_list.size() - 1];
        for (size_t j = 0; j < input_list.size(); ++j)
        {
            uint32_t end = input_list[j];

            float d0 = dot(vertices[start].clip_coordinates, plane);
            float d1 = dot(vertices[end].clip_coordinates, plane);

            if (d0 > 0)
            {
                if (d1 > 0)
                {
                    out_list.push_back(end);
                }
                else
                {
                    float a = d0 / (d0 - d1);
                    out_list.push_back(vertices.interpolate(start, end, a));
                }
            }
            else if (d1 > 0)
            {
                float a = d0 / (d0 - d1);
                out_list.push_back(vertices.interpolate(start, end, a));
                out_list.push_back(end);
            }
            start = end;
        }
    }

    std::swap(input_list, out_list);
}
