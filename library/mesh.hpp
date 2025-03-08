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
#include "library.hpp"

class Mesh;

/**
 * A set of indexes.
 */
struct Triangle
{
    const Mesh *owner;
    int indices[3];

    Triangle() = delete;
    Triangle(const Mesh *mesh, int i1, int i2, int i3) : owner(mesh), indices{i1, i2, i3} {}

    int at(size_t i) const { return indices[i]; }
    int operator[](size_t i) const { return indices[i]; }

	friend std::ostream &operator<<(std::ostream &os, const Triangle &rhs);
};


/**
 * A collection of faces and vertices.
 */
class Mesh
{
private:
    size_t count;
    std::vector<Vec4> vertices;
    std::vector<Vec3> textures;
    std::vector<Vec4> normals;

    /**
     * A vertex buffer containing 3 indices per face.
     */
    std::vector<int> elements;

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

    size_t size() const { return count; }
    size_t vertex_size() const { return vertices.size(); }

    Triangle at(size_t i) const
    {
        size_t index = i * 3;
        return {this, elements[index + 0], elements[index + 1], elements[index + 2]};
    }

    Triangle operator[](size_t i) const { return at(i); }

    const Vec4 &get_vertex(size_t i) const { return vertices[i]; }
    const Vec3 &get_texture(size_t i) const { return textures[i]; }
    const Vec4 &get_normal(size_t i) const { return normals[i]; }
};

struct Vertex
{
    Vec4 world, clip, normal;
    Vec3 screen, texture;
};

/**
 * Clips the given vertices using the Sutherland-Hodgman algorithm.
 * Modifies `input_list`.
 */
void sutherland_hodgman(std::vector<Vertex> &input_list);
