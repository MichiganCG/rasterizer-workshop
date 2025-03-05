#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "vectors.hpp"
#include "library.hpp"

struct VertexData
{
    Vec3 screen_coordinate;
    Vec3 texture_coordinate;
    Vec4 world;
    Vec4 clip;
    Vec4 normal;
};

/**
 * A collection of faces and vertices.
 */
class Mesh
{
public:
    Mesh() = delete;

    /**
     * Constructs the mesh by loading the given file.
     */
    Mesh(const std::string &file_name) { load_file(file_name); }

    /**
     * A struct containing a list of vertices, texture coordinates, and normals.
     */
    struct Face
    {
        size_t size;
        const Mesh *owner;
        std::vector<int> vertex_indices;
        std::vector<int> texture_indices;
        std::vector<int> normal_indices;

        Face() = delete;
        Face(const Mesh *mesh, size_t size) : size(size), owner(mesh), vertex_indices(size) {}

        const Vec4 &get_vertex(size_t i) const { return owner->vertices[vertex_indices[i]]; }
        const Vec3 &get_texture(size_t i) const { return owner->textures[texture_indices[i]]; }
        const Vec4 &get_normal(size_t i) const { return owner->normals[normal_indices[i]]; }
    };

    /**
     * Loads the faces of the mesh using a Wavefront .obj file.
     */
    void load_file(const std::string &file_name);

    size_t size() const { return faces.size(); }

    const Face &at(size_t i) const { return faces[i]; }
    Face &at(size_t i) { return faces[i]; }

    const Face &operator[](size_t i) const { return at(i); }
    Face &operator[](size_t i) { return at(i); }

    std::vector<Face>::iterator begin() { return faces.begin(); }
    std::vector<Face>::iterator end() { return faces.end(); }

private:
    std::vector<Vec4> vertices;
    std::vector<Vec3> textures;
    std::vector<Vec4> normals;

    std::vector<Face> faces;

    void fix_normals();
};

/**
 * Clips the given polygon using the Sutherland-Hodgman algorithm.
 * Modifies the 'vertex_list'.
 */
void sutherland_hodgman(std::vector<VertexData> &vertex_list);
