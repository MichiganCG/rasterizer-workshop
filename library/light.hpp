#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "vectors.hpp"
#include "mesh.hpp"
#include "library.hpp"

class Light
{
public:
    Color color;
    Vec3 direction;

    Light(Color color, Vec3 direction) : color(color), direction(direction) {}
    virtual ~Light() = default;
};

class DirectionalLight : public Light
{
public:
    DirectionalLight(Color color, Vec3 direction) : Light(color, direction) {}
};

/**
 * A collection of lights in the scene.
 */
class LightCollection
{
    std::vector<Light *> lights;

public:
    void push_back(DirectionalLight *light) { lights.push_back(light); }

    std::vector<Light *>::iterator begin() { return lights.begin(); }
    std::vector<Light *>::iterator end() { return lights.end(); }
};

/**
 * Basic material class using Wavefront .mtl files.
 */
class Material
{
    float shininess;
    Color ambient, diffuse, specular;

public:
    Material() : shininess(0), ambient{0}, diffuse{0}, specular{0} {}
    Material(const std::string &file_name) { load_file(file_name); }

    void load_file(const std::string &file_name);

    Color get_color(Vec3 &point, Vec3 &normal, LightCollection &lights);
};

/**
 * Uses the Digital Differential Analyzer (DDA) method to draw a line from 'start' to 'end'.
 */
void draw_line(Image &image, Vec2 &start, Vec2 &end);

/**
 * Uses barycentric coordinates to fill a triangle.
 */
void fill_barycentric(Image &image, DepthBuffer &depth, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2);

/**
 * Uses barycentric coordinates to fill a triangle.
 * Uses the material and light sources provided to determine color.
 */
void draw_barycentric(Image &image, DepthBuffer &depth, Material &mat, LightCollection &lights, Triangle &triangle);
