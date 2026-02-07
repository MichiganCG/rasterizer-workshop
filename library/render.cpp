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

#include "render.hpp"

constexpr double epsilon = -1E-5;

void draw_line(Image &image, Vec3 &start, Vec3 &end)
{
    float u, v, du, dv, step;
    du = end.x - start.x;
    dv = end.y - start.y;

    if (std::abs(du) >= std::abs(dv))
        step = std::abs(du);
    else
        step = std::abs(dv);

    du = du / step;
    dv = dv / step;
    u = start.x;
    v = start.y;
    int i = 0;
    while (i <= step)
    {
        image.set_pixel(std::round(u), std::round(v), {1, 1, 1});
        u = u + du;
        v = v + dv;
        ++i;
    }
}

void parallel_bounding_box(const std::function<void (uint32_t, uint32_t)> &action, const Vec3 &s0, const Vec3 &s1, const Vec3 &s2)
{
    // Calculate the bounding box around this triangle
    uint32_t minu = std::round(std::min({s0.x, s1.x, s2.x}));
    uint32_t maxu = std::round(std::max({s0.x, s1.x, s2.x}));
    uint32_t minv = std::round(std::min({s0.y, s1.y, s2.y}));
    uint32_t maxv = std::round(std::max({s0.y, s1.y, s2.y}));

    // Calculate the width and height of the bounding box
    uint32_t w = maxu - minu, h = maxv - minv;

    auto wrapper = [&](uint32_t i)
    {
        // Calculate the pixel coordinates
        uint32_t u = i % w + minu, v = i / w + minv;

        action(u, v);
    };

    parallel_for(0, w * h, wrapper, false);
}

inline Vec3 get_barycentric(const Vec3 &p, const Vec3 &s0, const Vec3 &s1, const Vec3 &s2)
{
    float area = (s1.x - s0.x) * (s2.y - s0.y) - (s2.x - s0.x) * (s1.y - s0.y);
    
    float b = ((s0.x - s2.x) * (p.y - s2.y) - (p.x - s2.x) * (s0.y - s2.y)) / area;
    float c = ((s1.x - s0.x) * (p.y - s0.y) - (p.x - s0.x) * (s1.y - s0.y)) / area;
    float a = 1 - b - c;

    return Vec3(a, b, c);
}

void iterate_shader(Image &image, DepthBuffer &depth, const std::function<Color(float, float, float)> shader, const Vec3 &s0, const Vec3 &s1, const Vec3 &s2)
{
    float z0 = s0.z, z1 = s1.z, z2 = s2.z; // get the depth of each vertex on the screen

    // Check the bounding box of the triangle
    auto action = [&](uint32_t u, uint32_t v)
    {
        // Get the center of the pixel
        Vec3 center(u + 0.5f, v + 0.5f);

        Vec3 bc = get_barycentric(center, s0, s1, s2);

        // Check if this pixel is in the triangle
        if (bc.x < epsilon || bc.y < epsilon || bc.z < epsilon) return;

        // Check if this pixel is closer to the screen
        float z = bc.x * z0 + bc.y * z1 + bc.z * z2;
        if (z > depth.at(u, v)) return;
        depth.at(u, v) = z;

        Color color = shader(bc.x, bc.y, bc.z);
        image.set_pixel(u, v, color);
    };

    parallel_bounding_box(action, s0, s1, s2);
}

void draw_barycentric(Image &image, DepthBuffer &depth, Color &color, Triplet triangle, VertexBuffer &vertices)
{
    const VertexBuffer::Vertex &v0 = vertices[triangle[0]], &v1 = vertices[triangle[1]], &v2 = vertices[triangle[2]];

    auto shader = [&](float a, float b, float c)
    {
        return color;
    };

    iterate_shader(image, depth, shader, v0.screen_coordinates, v1.screen_coordinates, v2.screen_coordinates);
}

void draw_barycentric(Image &image, DepthBuffer &depth, const Material &material, const Camera &camera, const LightCollection &lights, Triplet triangle, VertexBuffer &vertices)
{
    const VertexBuffer::Vertex &v0 = vertices[triangle[0]], &v1 = vertices[triangle[1]], &v2 = vertices[triangle[2]];
    float w0 = v0.clip_coordinates.w, w1 = v1.clip_coordinates.w, w2 = v2.clip_coordinates.w;

    auto shader = [&](float a, float b, float c)
    {
        // Correct for the perspective. https://www.cs.ucr.edu/~craigs/courses/2020-fall-cs-130/lectures/perspective-correct-interpolation.pdf
        float aw = a * w0, bw = b * w1, cw = c * w2;
        float w = 1.0f / (aw + bw + cw);

        // Interpolate across all of the vertex values
        Vec4 world =       w * (v0.world_coordinates   * aw + v1.world_coordinates   * bw + v2.world_coordinates   * cw);
        Vec4 normal = normalize(v0.world_normals       * aw + v1.world_normals       * bw + v2.world_normals       * cw);
        Vec3 texture =     w * (v0.texture_coordinates * aw + v1.texture_coordinates * bw + v2.texture_coordinates * cw);

        // Set the color using the material and lights
        return material.get_color(world, normal, texture, lights, camera.position);
    };

    iterate_shader(image, depth, shader, v0.screen_coordinates, v1.screen_coordinates, v2.screen_coordinates);
}