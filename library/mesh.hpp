#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "vectors.hpp"
#include "library.hpp"

class Mesh
{
public:

    Mesh() {}
    Mesh(const std::string &file_name) {
        load_file(file_name);
    }

    struct Triangle
    {
        const Mesh *mesh;
        int verts[3];

        Triangle(const Mesh *mesh, int v1, int v2, int v3) : mesh(mesh), verts{v1, v2, v3} {}
        const Vec3 &operator[](size_t i) const { return mesh->vertices[verts[i]]; }
    };

    void load_file(const std::string &file_name);

    size_t size() { return triangles.size(); }

    std::vector<Triangle>::iterator begin() { return triangles.begin(); }
    std::vector<Triangle>::iterator end() { return triangles.end(); }

private:
    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<Triangle> triangles;
};

struct Triangle
{
    int verts[3];
};

std::vector<Triangle> triangulate(std::vector<Vec3> &polygon);

struct Plane
{
    Vec3 point;
    Vec3 normal;
};

std::vector<Vec3> sutherland_hodgman(std::vector<Vec3> &input_list, const std::vector<Plane> &clipping_planes);

void DDA(Image &image, Vec2 &start, Vec2 &end);

// https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
Vec2 get_barycentric(Vec2 &p, Vec2 &t1, Vec2 &t2, Vec2 &t3);
void draw_barycentric(Image &image, Vec2 &t1, Vec2 &t2, Vec2 &t3);