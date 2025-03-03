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

class DepthBuffer
{
private:
    uint32_t width, height;
    std::vector<float> data;

public:
    DepthBuffer(uint32_t width, uint32_t height) : width(width), height(height), data(width * height, Infinity) {}

    float at(uint32_t x, uint32_t y) const { return data[y * width + x]; };
    float &at(uint32_t x, uint32_t y) { return data[y * width + x]; };
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
    Mesh() {}

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

struct Triangle
{
    Vec2 &v0, &v1, &v2;
};

/**
 * Clips the given polygon using the Sutherland-Hodgman algorithm.
 * @returns A list of clipped vertices
 */
std::vector<Vec3> sutherland_hodgman(std::vector<Vec3> &input_list, const std::vector<Plane> &clipping_planes);

/**
 * Uses the Digital Differential Analyzer method to draw a line from 'start' to 'end'.
 */
void draw_line(Image &image, Vec2 &start, Vec2 &end);

/**
 * Uses barycentric coordinates to fill a triangle.
 */
void draw_barycentric(Image &image, DepthBuffer &depth, Triangle &triangle);