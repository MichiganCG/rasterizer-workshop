#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "vectors.hpp"
#include "library.hpp"

struct Vertex
{
    Vec3 point;
    Vec3 normal;
    Vec2 screen;
    Vec2 texture;
};

/**
 * A collection of faces and vertices.
 */
class Mesh
{
public:
    /**
     * Default constructor.
     */
    Mesh() = delete;

    /**
     * Constructs the mesh by loading the given file.
     */
    Mesh(const std::string &file_name)
    {
        load_file(file_name);
    }

    /**
     * A struct containing a list of vertices, texture coordinates, and normals.
     */
    struct Face
    {
        const Mesh *owner;
        size_t size;
        std::vector<int> vertex_indices;
        std::vector<int> texture_indices;
        std::vector<int> normal_indices;

        Face() = delete;
        Face(const Mesh *mesh, size_t size) : owner(mesh), size(size), vertex_indices(size) {}

        const Vec3 &get_vertex(size_t i) const { return owner->vertices[vertex_indices[i]]; }
        const Vec2 &get_texture(size_t i) const { return owner->textures[texture_indices[i]]; }
        const Vec3 &get_normal(size_t i) const { return owner->normals[normal_indices[i]]; }
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
    std::vector<Vec3> vertices;
    std::vector<Vec2> textures;
    std::vector<Vec3> normals;

    std::vector<Face> faces;

    void fix_normals()
    {
        for (auto &face : faces)
        {
            if (face.normal_indices.size() == 0)
            {
                const Vec3 &v0 = face.get_vertex(0), &v1 = face.get_vertex(1), &v2 = face.get_vertex(2);

                // Compute face normal
                Vec3 normal = normalize(cross(v1 - v0, v2 - v0));
                normal.w = 0;

                int index = (int)normals.size();
                normals.push_back(normal);

                for (size_t i = 0; i < face.size; ++i)
                    face.normal_indices.push_back(index);
            }
        }
    }
};

/**
 * Clips the given polygon using the Sutherland-Hodgman algorithm.
 * Modifies the 'vertex_list'.
 */
void sutherland_hodgman_clip(std::vector<Vertex> &vertex_list);
