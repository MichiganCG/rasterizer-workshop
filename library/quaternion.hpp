#pragma once

#include "vectors.hpp"

#include <ostream>

// https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html

class Quaternion
{
public:
    float w, x, y, z;

    // Creates an identity quaternion with no rotation
    Quaternion();
    // Creates a quaternion with the specified components
    Quaternion(float w, float x, float y, float z);
    // Creates a quaternion using the given axis-angle representation
    Quaternion(const Vec3 &axis, float angle);
    // Copy ctor
    Quaternion(const Quaternion &other);
    Quaternion &operator=(const Quaternion &other);

    // Rotates (1, 0, 0)
    Vec3 right();
    // Rotates (0, 1, 0)
    Vec3 up();
    // Rotates (0, 0, 1)
    Vec3 forward();

    // Returns the imaginary component (bivector)
    Vec3 bivector();

    // Computes the axis-angle representation of this rotation
    void axis_angle(Vec3 &axis_out, float &angle_out);
    // Applies the axis-angle representation to this rotation
    Quaternion &rotate(const Vec3 &axis, float angle);

    /**
     * Mutiplication of two quaternions is associative, but no commutative
     * A rotation by A followed by a rotation by B is equal to C = A * B
     */
    Quaternion &operator*=(const Quaternion &rhs);
    Quaternion &operator*=(float rhs);

    friend std::ostream &operator<<(std::ostream &, const Quaternion &);
};

Quaternion operator*(Quaternion lhs, Quaternion rhs) { return (lhs *= rhs); }
Quaternion operator*(Quaternion lhs, float rhs) { return (lhs *= rhs); }

float dot(const Quaternion &, const Quaternion &);
float magnitude_squared(const Quaternion &);
float magnitude(const Quaternion &);

/**
 * Implementation from https://stackoverflow.com/questions/11667783/quaternion-and-normalization
 * @return A unit quaternion with the same rotation
 */
Quaternion normalize(const Quaternion &);

/**
 * @return The inverse of the rotation
 */
Quaternion conjugate(const Quaternion &);