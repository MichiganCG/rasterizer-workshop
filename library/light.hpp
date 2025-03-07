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
    Color color;

public:
    Light(Color color) : color(color) {}
    virtual ~Light() = default;

    virtual const Color &get_color() const { return color; }
    virtual Vec4 get_direction(const Vec4 &point) const { return Vec4::ZERO; }
    virtual float get_attenuation(const Vec4 &point) const { return 1; }
};

/**
 * A light source that is infinitely far away from the surface.
 * Assumes that the light rays are all parallel.
 */
class DirectionalLight : public Light
{
    Vec4 direction;

public:
    DirectionalLight(Color color, Vec4 direction) : Light(color), direction(-normalize(direction)) {}

    Vec4 get_direction(const Vec4 &point) const override;
};

/**
 * A light source at a specific point.
 */
class PointLight : public Light
{
    float k;
    Vec4 position;

public:
    PointLight(Color color, float k, Vec4 position) : Light(color), k(k), position(position) {}

    Vec4 get_direction(const Vec4 &point) const override;
    float get_attenuation(const Vec4 &point) const override;
};

/**
 * A light source at a specific point, shining in the given direction and angle.
 */
class SpotLight : public Light
{
    float angle, taper;
    Vec4 direction;
    Vec4 position;

public:
    SpotLight(Color color, float angle, float taper, Vec4 direction, Vec4 position) : Light(color), angle(std::cos(angle)), taper(taper), direction(normalize(direction)), position(position) {}

    Vec4 get_direction(const Vec4 &point) const override;
    float get_attenuation(const Vec4 &point) const override;
};

/**
 * A collection of lights in the scene.
 */
class LightCollection
{
    Color ambient;
    std::vector<Light *> lights;

public:
    LightCollection() : ambient{0.01, 0.01, 0.01} {}
    LightCollection(const Color &ambient) : ambient{ambient} {}

    const Color &get_ambient() { return ambient; }

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
    bool textured;
    float shininess;
    Color ambient, diffuse, specular;
    Image texture;

public:
    Material() : textured(false), shininess(0), ambient{0}, diffuse{0}, specular{0} {}
    Material(const std::string &file_name) : textured(false) { load_file(file_name); }

    void load_file(const std::string &file_name);

    Color get_color(const Vec4 &point, const Vec4 &normal, const Vec3 &texture_coordinates, LightCollection &lights);
};

/**
 * Uses the Digital Differential Analyzer (DDA) method to draw a line from 'start' to 'end'.
 */
void draw_line(Image &image, const Vec3 &start, const Vec3 &end);

/**
 * Uses barycentric coordinates to fill a triangle.
 */
void draw_barycentric(Image &image, DepthBuffer &depth, Color &color, const VertexData &vertex0, const VertexData &vertex1, const VertexData &vertex2);

/**
 * Uses barycentric coordinates to fill a triangle.
 * Uses the material and light sources provided to determine color.
 */
void draw_barycentric(Image &image, DepthBuffer &depth, Material &mat, LightCollection &lights, const VertexData &vertex0, const VertexData &vertex1, const VertexData &vertex2);
