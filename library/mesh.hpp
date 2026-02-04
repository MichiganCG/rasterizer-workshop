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
#include <cassert>

#include "vectors.hpp"
#include "library.hpp"

/**
 * A set of three indices.
 */
struct Triplet
{
    uint32_t indices[3];

    Triplet() = delete;
    Triplet(uint32_t i1, uint32_t i2, uint32_t i3) : indices{i1, i2, i3} {}

    uint32_t at(size_t i) const { return indices[i]; }
    uint32_t operator[](size_t i) const { return indices[i]; }

    friend std::ostream &operator<<(std::ostream &os, const Triplet &rhs);
};

/**
 * A collection of faces and vertices.
 */
class Mesh
{
private:
    size_t count;
    std::vector<Vec4> vertices;
    std::vector<Vec4> normals;
    std::vector<Vec3> textures;

    /**
     * A vertex buffer containing 3 indices per face.
     */
    std::vector<uint32_t> elements;

public:
    Mesh() = delete;

    /**
     * Constructs the mesh from the given file.
     */
    Mesh(const std::string &file_name) { load_file(file_name); }

    /**
     * Loads the faces of the mesh using a Wavefront .obj file.
     */
    void load_file(const std::string &file_name);

    // Returns the number of triangles
    size_t size() const { return count; }
    // Returns the number of vertices
    size_t vertex_size() const { return vertices.size(); }

    Triplet at(size_t i) const
    {
        size_t index = i * 3;
        return {elements[index + 0], elements[index + 1], elements[index + 2]};
    }

    Triplet operator[](size_t i) const { return at(i); }

    const Vec4 &get_vertex(size_t i) const { return vertices[i]; }
    const Vec3 &get_texture(size_t i) const { return textures[i]; }
    const Vec4 &get_normal(size_t i) const { return normals[i]; }

    void smooth_normals();
};

class VertexData{
public:
    VertexData(size_t size) : data(size) {}

    struct Vertex {
        Vec4 world_coordinates;
        Vec4 world_normals;
        Vec4 clip_coordinates;
        Vec3 texture_coordinates;
        Vec3 screen_coordinates;
    };

    size_t size() const { return data.size(); }

    Vertex &at(size_t i) { return data[i]; }

    Vertex &operator[](size_t i) { return at(i); }

    uint32_t interpolate(uint32_t start, uint32_t end, float a) {
        data.emplace_back(
            data[start].world_coordinates   * (1 - a) + data[end].world_coordinates   * (a),
            data[start].world_normals       * (1 - a) + data[end].world_normals       * (a),
            data[start].clip_coordinates    * (1 - a) + data[end].clip_coordinates    * (a),
            data[start].texture_coordinates * (1 - a) + data[end].texture_coordinates * (a)
        );
        return data.size() - 1;
    }
private:
    std::vector<Vertex> data;
};

/**
 * Clips the given vertices using the Sutherland-Hodgman algorithm.
 * Modifies `input_list`.
 */
void sutherland_hodgman(std::vector<uint32_t> &input_list, VertexData &vertices);
