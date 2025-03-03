#pragma once

#include "vectors.hpp"
#include "mesh.hpp"
#include "library.hpp"

/**
 * Uses the Digital Differential Analyzer method to draw a line from 'start' to 'end'.
 */
void draw_line(Image &image, Vec2 &start, Vec2 &end);

/**
 * Uses barycentric coordinates to fill a triangle.
 */
void draw_barycentric(Image &image, DepthBuffer &depth, Vec2 &v0, Vec2 &v1, Vec2 &v2);

void draw_barycentric(Image &image, DepthBuffer &depth, Vec2 &v0, Vec2 &v1, Vec2 &v2, Vec3 &n0, Vec3 &n1, Vec3 &n2);