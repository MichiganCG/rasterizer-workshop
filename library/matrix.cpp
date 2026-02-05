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

#include "matrix.hpp"

Matrix4 &Matrix4::operator+=(const Matrix4 &other)
{
    for (size_t i = 0; i < 16; ++i)
        data[i] += other.data[i];

    return *this;
}

Matrix4 &Matrix4::operator-=(const Matrix4 &other)
{
    for (size_t i = 0; i < 16; ++i)
        data[i] -= other.data[i];

    return *this;
}

Matrix4 &Matrix4::operator*=(const Matrix4 &rhs)
{
    Matrix4 matrix;
    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            for (size_t k = 0; k < 4; ++k)
            {
                matrix.at(i, j) += (at(i, k) * rhs.at(k, j));
            }
        }
    }
    return (*this = matrix);
}

Vec4 operator*(const Matrix4 &lhs, const Vec4 &rhs)
{
    Vec4 vector;
    vector.x = rhs.x * lhs.at(0, 0) + rhs.y * lhs.at(0, 1) + rhs.z * lhs.at(0, 2) + rhs.w * lhs.at(0, 3);
    vector.y = rhs.x * lhs.at(1, 0) + rhs.y * lhs.at(1, 1) + rhs.z * lhs.at(1, 2) + rhs.w * lhs.at(1, 3);
    vector.z = rhs.x * lhs.at(2, 0) + rhs.y * lhs.at(2, 1) + rhs.z * lhs.at(2, 2) + rhs.w * lhs.at(2, 3);
    vector.w = rhs.x * lhs.at(3, 0) + rhs.y * lhs.at(3, 1) + rhs.z * lhs.at(3, 2) + rhs.w * lhs.at(3, 3);
    return vector;
}

Matrix4 &Matrix4::operator*=(float rhs)
{
    for (size_t i = 0; i < 16; ++i)
    {
        data[i] *= rhs;
    }
    return *this;
}

Matrix4 operator+(Matrix4 lhs, const Matrix4 &rhs) { return (lhs += rhs); }
Matrix4 operator-(Matrix4 lhs, const Matrix4 &rhs) { return (lhs -= rhs); }
Matrix4 operator*(Matrix4 lhs, const Matrix4 &rhs) { return (lhs *= rhs); }
Matrix4 operator*(Matrix4 lhs, float rhs) { return (lhs *= rhs); }
Vec4 operator*(const Vec4 &lhs, const Matrix4 &rhs) { return rhs * lhs; }

std::ostream &operator<<(std::ostream &os, const Matrix4 &rhs)
{
    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            os << rhs[i][j] << " ";
        }
    }
    return os;
}

Matrix4 look_at(const Vec3 &eye, const Vec3 &target, const Vec3 &up_dir)
{
    Vec3 forward = eye - target;
    forward = normalize(forward);

    Vec3 left = cross(up_dir, forward);
    left = normalize(left);

    Vec3 up = cross(forward, left);

    Matrix4 matrix;
    matrix.identity();

    matrix.at(0, 0) = left.x;
    matrix.at(0, 1) = left.y;
    matrix.at(0, 2) = left.z;
    matrix.at(1, 0) = up.x;
    matrix.at(1, 1) = up.y;
    matrix.at(1, 2) = up.z;
    matrix.at(2, 0) = forward.x;
    matrix.at(2, 1) = forward.y;
    matrix.at(2, 2) = forward.z;

    matrix.at(0, 3) = -left.x * eye.x - left.y * eye.y - left.z * eye.z;
    matrix.at(1, 3) = -up.x * eye.x - up.y * eye.y - up.z * eye.z;
    matrix.at(2, 3) = -forward.x * eye.x - forward.y * eye.y - forward.z * eye.z;

    return matrix;
}

void translate(Matrix4 &matrix, const Vec3 &translation)
{
    matrix.at(0, 3) = translation.x;
    matrix.at(1, 3) = translation.y;
    matrix.at(2, 3) = translation.z;
}

Matrix4 translate(const Vec3 &translation)
{
    Matrix4 matrix;
    matrix.identity();
    translate(matrix, translation);
    return matrix;
}

void rotate(Matrix4 &matrix, const Quaternion &rotation)
{
    float r = rotation.w, i = rotation.x, j = rotation.y, k = rotation.z;
    matrix.at(0, 0) = 1 - (2 * (j * j + k * k));
    matrix.at(0, 1) = 2 * (i * j - r * k);
    matrix.at(0, 2) = 2 * (i * k + r * j);
    matrix.at(1, 0) = 2 * (i * j + r * k);
    matrix.at(1, 1) = 1 - (2 * (i * i + k * k));
    matrix.at(1, 2) = 2 * (j * k - r * i);
    matrix.at(2, 0) = 2 * (i * k - r * j);
    matrix.at(2, 1) = 2 * (j * k + r * i);
    matrix.at(2, 2) = 1 - (2 * (i * i + j * j));
    matrix.at(3, 3) = 1;
}

Matrix4 rotate(const Quaternion &rotation)
{
    Matrix4 matrix;
    matrix.identity();
    rotate(matrix, rotation);
    return matrix;
}

void scale(Matrix4 &matrix, const Vec3 &scales)
{
    matrix.at(0, 0) *= scales.x;
    matrix.at(1, 1) *= scales.y;
    matrix.at(2, 2) *= scales.z;
}

Matrix4 scale(const Vec3 &scales)
{
    Matrix4 matrix;
    matrix.identity();
    scale(matrix, scales);
    return matrix;
}

Matrix4 quick_inverse(const Matrix4 &input)
{
    Matrix4 matrix;
    matrix.at(0, 0) = input.at(0, 0);
    matrix.at(0, 1) = input.at(1, 0);
    matrix.at(0, 2) = input.at(2, 0);
    matrix.at(1, 0) = input.at(0, 1);
    matrix.at(1, 1) = input.at(1, 1);
    matrix.at(1, 2) = input.at(2, 1);
    matrix.at(2, 0) = input.at(0, 2);
    matrix.at(2, 1) = input.at(1, 2);
    matrix.at(2, 2) = input.at(2, 2);
    matrix.at(0, 3) = -(input.at(0, 3) * input.at(0, 0) + input.at(1, 3) * input.at(1, 0) + input.at(2, 3) * input.at(2, 0));
    matrix.at(1, 3) = -(input.at(0, 3) * input.at(0, 1) + input.at(1, 3) * input.at(1, 1) + input.at(2, 3) * input.at(2, 1));
    matrix.at(2, 3) = -(input.at(0, 3) * input.at(0, 2) + input.at(1, 3) * input.at(1, 2) + input.at(2, 3) * input.at(2, 2));
    matrix.at(3, 3) = 1;
    return matrix;
}

Matrix4 orthographic_projection(float right, float top, float near, float far)
{
    Matrix4 matrix;
    matrix.at(0, 0) = 1 / right;
    matrix.at(1, 1) = 1 / top;
    matrix.at(2, 2) = 1 / (near - far);
    matrix.at(2, 3) = ((far + near) / (near - far) + 1) * 0.5f; 
    matrix.at(3, 3) = 1;
    return matrix;
}

// https://www.mauriciopoppe.com/notes/computer-graphics/viewing/projection-transform/
Matrix4 perspective_projection(float fov, float aspect_ratio, float near, float far)
{
    float tangent = 1.0f / std::tan((fov / 2) * (Pi / 180));

    Matrix4 matrix;
    matrix.at(0, 0) = tangent / aspect_ratio;      // map x between [-1, 1]
    matrix.at(1, 1) = tangent;                     // map y between [-1, 1]
    matrix.at(2, 2) = (far) / (near - far);        // adjusted to map z between [0, 1]
    matrix.at(2, 3) = (near * far) / (near - far); // adjusted to map z between [0, 1]
    matrix.at(3, 2) = -1;                          // set w = -z
    return matrix;
}

Matrix4 viewport(uint32_t width, uint32_t height)
{
    float half_width = width / 2;
    float half_height = height / 2;

    Matrix4 matrix;
    matrix.at(0, 0) = half_width;
    matrix.at(1, 1) = -half_height;
    matrix.at(2, 2) = 1;
    matrix.at(0, 3) = half_width;
    matrix.at(1, 3) = half_height;
    matrix.at(3, 3) = 1;
    return matrix;
}
