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

    Light(Color color, Vec3 direction) : color(color), direction(normalize(direction)) {}
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
    void push_back(Light *light) { lights.push_back(light); }
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

    Color get_color(const Vec3 &point, const Vec3 &normal, LightCollection &lights);
};

/**
 * Uses the Digital Differential Analyzer (DDA) method to draw a line from 'start' to 'end'.
 */
void draw_line(Image &image, const Vec3 &start, const Vec3 &end);

/**
 * Uses barycentric coordinates to fill a triangle.
 */
void draw_barycentric(Image &image, DepthBuffer &depth, Color &color, const Vertex &vertex0, const Vertex &vertex1, const Vertex &vertex2);

/**
 * Uses barycentric coordinates to fill a triangle.
 * Uses the material and light sources provided to determine color.
 */
void draw_barycentric(Image &image, DepthBuffer &depth, Material &mat, LightCollection &lights, const Vertex &vertex0, const Vertex &vertex1, const Vertex &vertex2);
