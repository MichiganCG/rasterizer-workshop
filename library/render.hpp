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

#pragma once

#include "vectors.hpp"
#include "mesh.hpp"
#include "light.hpp"
#include "scene.hpp"
#include "library.hpp"

/**
 * Uses the Digital Differential Analyzer (DDA) method to draw a line from 'start' to 'end'.
 */
void draw_line(Image &image, const Vec3 &start, const Vec3 &end);

void parallel_bounding_box(const std::function<void (uint32_t, uint32_t)> &action, const Vec3 &s0, const Vec3 &s1, const Vec3 &s2);

inline Vec3 get_barycentric(const Vec3 &p, const Vec3 &s0, const Vec3 &s1, const Vec3 &s2);

void iterate_shader(Image &image, DepthBuffer &depth, const std::function<Color(float, float, float)> shader, const Vec3 &s0, const Vec3 &s1, const Vec3 &s2);

/**
 * Uses barycentric coordinates to fill a triangle with the given color.
 */
void draw_barycentric(Image &image, DepthBuffer &depth, Color &color, Triplet triangle, VertexBuffer &vertices);

/**
 * Uses the object's material and all light sources provided to determine the color of each pixel.
 */
void draw_barycentric(Image &image, DepthBuffer &depth, const Material &mat, const Camera &camera, const LightCollection &lights, Triplet triangle, VertexBuffer &vertices);