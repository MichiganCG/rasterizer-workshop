#include "quaternion.hpp"
#include "library.hpp"

#define epsilon 8E-7f

Quaternion::Quaternion() : w(1), x(0), y(0), z(0) {}

Quaternion::Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

Quaternion::Quaternion(const Vec3 &axis, float angle) : Quaternion()
{
    Vec3 normal_axis = normalize(axis);
    w = std::cos(angle * 0.5);
    float sin_half = std::sin(angle * 0.5);
    x = normal_axis.x * sin_half;
    y = normal_axis.y * sin_half;
    z = normal_axis.z * sin_half;
}

Quaternion::Quaternion(const Quaternion &other) : w(other.w), x(other.x), y(other.y), z(other.z) {}

Quaternion &Quaternion::operator=(const Quaternion &other) &
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
    return (*this *= Quaternion(axis, angle));
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

Quaternion operator*(Quaternion lhs, Quaternion rhs) { return (lhs *= rhs); }

Quaternion &Quaternion::operator*=(float rhs)
{
    w *= rhs;
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}

Quaternion operator*(Quaternion lhs, float rhs) { return (lhs *= rhs); }

std::ostream &operator<<(std::ostream &os, const Quaternion &rhs)
{
    os << "( " << rhs.w << " " << rhs.x << " " << rhs.y << " " << rhs.z << " )";
    return os;
}

float dot(const Quaternion &lhs, const Quaternion &rhs)
{
    return lhs.w * rhs.w + lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

float magnitude_squared(const Quaternion &input)
{
    return dot(input, input);
}

float magnitude(const Quaternion &input)
{
    return std::sqrt(magnitude_squared(input));
}

Quaternion normalize(const Quaternion &input)
{
    float magnitude_sqrd = magnitude_squared(input);
    if (std::abs(1.0f - magnitude_sqrd) < 2.107342E-08)
        return input * (2.0f / (1.0f + magnitude_sqrd));
    return input * (1.0f / std::sqrt(magnitude_sqrd));
}

Quaternion conjugate(const Quaternion &input)
{
    return {input.w, -input.x, -input.y, -input.z};
}