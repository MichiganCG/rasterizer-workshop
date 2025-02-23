#include "quaternion.hpp"
#include "library.hpp"

#define epsilon 8E-7f

Quaternion::Quaternion() : w(0.0f), x(0.0f), y(0.0f), z(1.0f) {}

Quaternion::Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

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

/**
 * Applies the quaternion to the vector (0, 0, 1).
 */
Vec3 Quaternion::forward()
{
    return {
        2 * (x * z + w * y),
        2 * (y * z - w * x),
        (2 * (x * x + y * y)) - 1};
}

/**
 * Applies the quaternion to the vector (0, 1, 0).
 */
Vec3 Quaternion::up()
{
    return {
        2 * (x * y - w * z),
        (2 * (x * x + z * z)) - 1,
        2 * (y * z + w * x)};
}

/**
 * Applies the quaternion to the vector (1, 0, 0).
 */
Vec3 Quaternion::right()
{
    return {
        (2 * (y * y + z * z)) - 1,
        2 * (x * y + w * z),
        2 * (x * z - w * y)};
}

/**
 * Returns the imaginary component, also known as the bivector.
 */
Vec3 Quaternion::bivector()
{
    return {x, y, z};
}

Quaternion &Quaternion::operator*=(const Quaternion &rhs)
{
    Quaternion temp;
    temp.w = (w * rhs.w) - (x * rhs.x) - (y * rhs.y) - (z * rhs.z);
    temp.x = (w * rhs.x) + (x * rhs.w) + (y * rhs.z) - (z * rhs.y);
    temp.y = (w * rhs.y) - (x * rhs.z) + (y * rhs.w) + (z * rhs.x);
    temp.z = (w * rhs.z) + (x * rhs.y) - (y * rhs.x) + (z * rhs.w);
    return (*this = temp);
}

Quaternion &Quaternion::operator/=(const float rhs)
{
    w /= rhs;
    x /= rhs;
    y /= rhs;
    z /= rhs;
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
    float squared = magnitude_squared(q);
    if (almost_zero(squared))
        return Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
    return q / std::sqrt(squared);
}

Quaternion conjugate(const Quaternion &q)
{
    return {q.w, -q.x, -q.y, -q.z};
}

void Quaternion::get_axis_angle(Vec3 &axis, float &angle)
{
    if (w > 1)
    {
        normalize(*this);
    }

    angle = 2 * std::acos(w);

    float s = std::sqrt(1 - w * w);
    if (s < epsilon)
    { // Avoid divide by zero.
        axis = bivector();
        return;
    }
    axis = bivector() / s;
}

Quaternion local_rotation(const Vec3 &axis, const float angle)
{
    float half_angle = angle * 0.5;
    return {
        std::cos(half_angle),
        axis.x * std::sin(half_angle),
        axis.y * std::sin(half_angle),
        axis.z * std::sin(half_angle)};
}

void Quaternion::rotate(const Vec3 &axis, const float angle)
{
    Quaternion local = local_rotation(normalize(axis), angle);
    *this *= local;
}