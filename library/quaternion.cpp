#include "quaternion.hpp"
#include "library.hpp"

#define epsilon 8E-7f

Quaternion::Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}

Quaternion::Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

Quaternion::Quaternion(const Vec3 &axis, float angle) : Quaternion()
{
    w = std::cos(angle * 0.5);
    float sin_half = std::sin(angle * 0.5);
    x = axis.x * sin_half;
    y = axis.y * sin_half;
    z = axis.z * sin_half;
}

Quaternion::Quaternion(const Quaternion &other) : w(other.w), x(other.x), y(other.y), z(other.z) {}

Quaternion &Quaternion::operator=(const Quaternion &other)
{
    if (this == &other)
        return *this;
    w = other.w;
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}

Vec3 Quaternion::right()
{
    return {
        1 - (2 * (y * y + z * z)),
        2 * (x * y + w * z),
        2 * (x * z - w * y)};
}

Vec3 Quaternion::up()
{
    return {
        2 * (x * y - w * z),
        1 - (2 * (x * x + z * z)),
        2 * (y * z + w * x)};
}

Vec3 Quaternion::forward()
{
    return {
        2 * (x * z + w * y),
        2 * (y * z - w * x),
        1 - (2 * (x * x + y * y))};
}

Vec3 Quaternion::bivector()
{
    return {x, y, z};
}

void Quaternion::axis_angle(Vec3 &axis_out, float &angle_out)
{
    Vec3 biv = bivector();
    float s = magnitude(biv);
    axis_out = biv * (1.0f / s);

    angle_out = 2 * std::atan2(s, w);
}

Quaternion &Quaternion::rotate(const Vec3 &axis, float angle)
{
    return (*this *= Quaternion(normalize(axis), angle));
}

Quaternion &Quaternion::operator*=(const Quaternion &rhs)
{
    Quaternion quaternion;
    quaternion.w = (w * rhs.w) - (x * rhs.x) - (y * rhs.y) - (z * rhs.z);
    quaternion.x = (w * rhs.x) + (x * rhs.w) - (y * rhs.z) + (z * rhs.y);
    quaternion.y = (w * rhs.y) + (x * rhs.z) + (y * rhs.w) - (z * rhs.x);
    quaternion.z = (w * rhs.z) - (x * rhs.y) + (y * rhs.x) + (z * rhs.w);
    return (*this = quaternion);
}

Quaternion &Quaternion::operator*=(float rhs)
{
    w *= rhs;
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const Quaternion &rhs)
{
    os << rhs.w << " " << rhs.x << " " << rhs.y << " " << rhs.z << std::endl;
    return os;
}

float dot(const Quaternion &lhs, const Quaternion &rhs)
{
    return lhs.w * rhs.w + lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

float magnitude_squared(const Quaternion &q)
{
    return dot(q, q);
}

float magnitude(const Quaternion &q)
{
    return std::sqrt(magnitude_squared(q));
}

Quaternion normalize(const Quaternion &q)
{
    float qmagsq = magnitude_squared(q);
    if (std::abs(1.0f - qmagsq) < 2.107342E-08)
        return q * (2.0f / (1.0f + qmagsq));
    return q * (1.0f / std::sqrt(qmagsq));
}

Quaternion conjugate(const Quaternion &q)
{
    return {q.w, -q.x, -q.y, -q.z};
}