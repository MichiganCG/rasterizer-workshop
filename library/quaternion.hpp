#pragma once

#include "vectors.hpp"

#include <ostream>

// https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html

class Quaternion
{
public:
    float w, x, y, z;

    Quaternion();
    Quaternion(float w, float x, float y, float z);
    Quaternion(const Vec3 &axis, float angle);
    Quaternion(const Quaternion &other);
    Quaternion &operator=(const Quaternion &other);

    Vec3 forward();
    Vec3 up();
    Vec3 right();
    Vec3 bivector();

    void axis_angle(Vec3 &axis_out, float &angle_out);

    Quaternion &rotate(const Vec3 &axis, float angle);

    Quaternion &operator*=(const Quaternion &rhs);
    Quaternion &operator*=(float rhs);

    friend std::ostream &operator<<(std::ostream &, const Quaternion &);
};

Quaternion operator*(Quaternion lhs, Quaternion rhs) { return (lhs *= rhs); }
Quaternion operator*(Quaternion lhs, float rhs) { return (lhs *= rhs); }

float dot(const Quaternion &, const Quaternion &);
float magnitude_squared(const Quaternion &);
float magnitude(const Quaternion &);

Quaternion normalize(const Quaternion &);
Quaternion conjugate(const Quaternion &);