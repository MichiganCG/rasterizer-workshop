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
 * A collection of triangle faces and vertices.
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

    struct Triangle
    {
        const Mesh *owner;
        int vertex_indices[3];
        int texture_indices[3];
        int normal_indices[3];

        Triangle(const Mesh *mesh) : owner(mesh) {}
        const Vec3 &get_vertex(size_t i) const { return owner->vertices[vertex_indices[i]]; }
        const Vec2 &get_texture(size_t i) const { return owner->textures[texture_indices[i]]; }
        const Vec3 &get_normal(size_t i) const { return owner->normals[normal_indices[i]]; }

        void set_vertices(int v0, int v1, int v2)
        {
            vertex_indices[0] = v0;
            vertex_indices[1] = v1;
            vertex_indices[2] = v2;
        }

        void set_textures(int t0, int t1, int t2)
        {
            texture_indices[0] = t0;
            texture_indices[1] = t1;
            texture_indices[2] = t2;
        }

        void set_normals(int n0, int n1, int n2)
        {
            normal_indices[0] = n0;
            normal_indices[1] = n1;
            normal_indices[2] = n2;
        }
    };

    /**
     * Loads the faces of the mesh using a Wavefront .obj file.
     */
    void load_file(const std::string &file_name);

    size_t size() { return triangles.size(); }

    const Triangle &at(size_t i) const { return triangles[i]; }
    Triangle &at(size_t i) { return triangles[i]; }

    const Triangle &operator[](size_t i) const { return at(i); }
    Triangle &operator[](size_t i) { return at(i); }

    std::vector<Triangle>::iterator begin() { return triangles.begin(); }
    std::vector<Triangle>::iterator end() { return triangles.end(); }

private:
    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<Vec2> textures;

    std::vector<Triangle> triangles;
};

/**
 * Clips the given polygon using the Sutherland-Hodgman algorithm.
 * @returns A list of clipped vertices
 */
std::vector<Vec3> sutherland_hodgman(std::vector<Vec3> &input_list, const std::vector<Plane> &clipping_planes);

/**
 * Digital Differential Analyzer.
 * Draws a line from 'start' to 'end'.
 */
void draw_DDA(Image &image, Vec2 &start, Vec2 &end);

/**
 * Uses barycentric coordinates to fill a triangle.
 */
void draw_barycentric(Image &image, DepthBuffer &depth, Vec2 &t1, Vec2 &t2, Vec2 &t3);