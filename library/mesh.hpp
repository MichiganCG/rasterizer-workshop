#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "vectors.hpp"
#include "library.hpp"

/**
 * A plane is defined by a point and normal vector.
 */
struct Plane
{
    Vec3 point;
    Vec3 normal;
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
        Face(const Mesh *mesh, size_t size) : owner(mesh), size(size), vertex_indices(size), texture_indices(size), normal_indices(size) {}

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
    std::vector<Vec3> normals;
    std::vector<Vec2> textures;

    std::vector<Face> faces;
};

class Triangle
{
    int indices[3];
    std::vector<Vec2> &points;
    std::vector<Vec3> &vertices;
    std::vector<Vec3> &normals;

public:
    Triangle() = default;
    Triangle(size_t index, std::vector<Vec2> &points, std::vector<Vec3> &vertices, std::vector<Vec3> &normals) : points(points), vertices(vertices), normals(normals)
    {
        indices[0] = 0;
        indices[1] = index;
        indices[2] = index + 1;
    }

    const Vec2 &point(size_t i) const { return points[indices[i]]; }
    const Vec3 &vertex(size_t i) const { return vertices[indices[i]]; }
    const Vec3 &normal(size_t i) const { return normals[indices[i]]; }
};

/**
 * Clips the given polygon using the Sutherland-Hodgman algorithm.
 * @returns A list of clipped vertices
 */
std::vector<Vec3> sutherland_hodgman(std::vector<Vec3> &input_list, const std::vector<Plane> &clipping_planes);
