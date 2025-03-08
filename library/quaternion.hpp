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

#include <ostream>

/**
 * An implementation of rotation quaternions.
 * @cite https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html
 */
class Quaternion
{
public:
    float w, x, y, z;

    /**
     * Default constructor.
     * Initializes an identity quaternion with no rotation `q = 1`.
     */
    Quaternion();

    /**
     * Constructs a quaternion of the form `q = w + xi + yj + zk`.
     */
    Quaternion(float w, float x, float y, float z);

    /**
     * Constructs a quaternion using the given axis-angle representation.
     */
    Quaternion(const Vec3 &axis, float angle);

    /**
     * Copy constructor.
     * Constructs a copy of the given quaternion.
     */
    Quaternion(const Quaternion &other);

    Quaternion &operator=(const Quaternion &other) &;

    /**
     * Applies the rotation of this quaternion to the vector `<1, 0, 0>`.
     * @returns The right direction vector.
     */
    Vec4 right();

    /**
     * Applies the rotation of this quaternion to the vector `<0, 1, 0>`.
     * @returns The up direction vector.
     */
    Vec4 up();

    /**
     * Applies the rotation of this quaternion to the vector `<0, 0, 1>`.
     * @returns The forward direction vector.
     */
    Vec4 forward();

    /**
     * @returns The imaginary component (bivector) of this quaternion
     */
    Vec4 bivector();

    /**
     * Computes the axis-angle representation of this rotation.
     * @param axis_out Sets equal to this quaternion's axis
     * @param angle_out Sets equal to this quaternion's angle
     */
    void axis_angle(Vec3 &axis_out, float &angle_out);

    /**
     * Constructs a new quaternion using the given axis-angle representation and multiplies
     * it with this quaternion to combine their rotations.
     * @returns The total rotation
     */
    Quaternion &rotate(const Vec3 &axis, float angle);

    /**
     * Multiplies two (2) quaternions `A` and `B`. This result in a quaternion that performs
     * a rotation by `A` followed by a rotation by `B`.
     * @note Mutiplication of two quaternions is associative, but not commutative.
     */
    Quaternion &operator*=(const Quaternion &rhs);
    Quaternion &operator*=(float rhs);

    friend std::ostream &operator<<(std::ostream &os, const Quaternion &rhs);
};

Quaternion operator*(Quaternion lhs, Quaternion rhs);
Quaternion operator*(Quaternion lhs, float rhs);

/**
 * Computes the dot product of two (2) quaternions.
 * @returns The dot product
 */
float dot(const Quaternion &lhs, const Quaternion &rhs);

/**
 * Computes the square of the magnitude of the given quaternion.
 * @returns The square of the magnitude
 */
float magnitude_squared(const Quaternion &input);

/**
 * Computes the magnitude of the given quaternion.
 * @returns The magnitude
 */
float magnitude(const Quaternion &input);

/**
 * Normalizes the quaternion to be of length one (1).
 * @cite https://stackoverflow.com/questions/11667783/quaternion-and-normalization
 * @return A unit quaternion with the same rotation
 */
Quaternion normalize(const Quaternion &input);

/**
 * Computes the conjugate of the quaternion.
 * @note For the special case of rotation quaternions, this is equal to its inverse.
 * @return The inverse of the rotation
 */
Quaternion conjugate(const Quaternion &input);