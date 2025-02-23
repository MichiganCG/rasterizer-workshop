#include "vectors.hpp"
#include "library.hpp"

Vec2::Vec2() : u(0.0f), v(0.0f), w(1.0f) {}
Vec2::Vec2(float u, float v) : u(u), v(v), w(1.0f) {}
Vec2::Vec2(float value) : u(value), v(value), w(1.0f) {}

Vec2::Vec2(const Vec2 &other) : u(other.u), v(other.v), w(1.0f) {}

Vec2 &Vec2::operator=(const Vec2 &other)
{
    if (this == &other)
        return *this;
    u = other.u;
    v = other.v;
    w = other.w;
    return *this;
}

Vec3::Vec3() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z), w(1.0f) {}
Vec3::Vec3(float value) : x(value), y(value), z(value), w(1.0f) {}

Vec3::Vec3(const Vec3 &other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

Vec3 &Vec3::operator=(const Vec3 &other)
{
    if (this == &other)
        return *this;
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    return *this;
}

Vec3 &Vec3::operator+=(const Vec3 &rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}

Vec3 &Vec3::operator-=(const Vec3 &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}

Vec3 &Vec3::operator*=(const Vec3 &rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;
    return *this;
}

Vec3 &Vec3::operator/=(const Vec3 &rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;
    return *this;
}

Vec3 &Vec3::operator*=(const Matrix4 &rhs)
{
    Vec3 temp;
    temp.x = x * rhs.at(0, 0) + y * rhs.at(1, 0) + z * rhs.at(2, 0) + w * rhs.at(3, 0);
    temp.y = x * rhs.at(0, 1) + y * rhs.at(1, 1) + z * rhs.at(2, 1) + w * rhs.at(3, 1);
    temp.z = x * rhs.at(0, 2) + y * rhs.at(1, 2) + z * rhs.at(2, 2) + w * rhs.at(3, 2);
    temp.w = x * rhs.at(0, 3) + y * rhs.at(1, 3) + z * rhs.at(2, 3) + w * rhs.at(3, 3);
    return (*this = temp);
}

Vec3 &Vec3::operator*=(const float rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
    w *= rhs;
    return *this;
}

Vec3 &Vec3::operator/=(const float rhs)
{
    x /= rhs;
    y /= rhs;
    z /= rhs;
    w /= rhs;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const Vec3 &rhs)
{
    os << rhs.x << " " << rhs.y << " " << rhs.z << " " << rhs.w << std::endl;
    return os;
}

float dot(const Vec3 &lhs, const Vec3 &rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

float abs_dot(const Vec3 &lhs, const Vec3 &rhs)
{
    return std::abs(dot(lhs, rhs));
}

float magnitude_squared(const Vec3 &v)
{
    return dot(v, v);
}

float magnitude(const Vec3 &v)
{
    return std::sqrt(magnitude_squared(v));
}

float lerp(const float a, const float b, const float t)
{
    return (1.0f - t) * a + b * t;
}

Vec3 normalize(const Vec3 &v)
{
    float squared = magnitude_squared(v);
    if (almost_zero(squared))
        return Vec3();
    return v / std::sqrt(squared);
}

Vec3 cross(const Vec3 &lhs, const Vec3 &rhs)
{
    return {lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x};
}

Vec3 lerp(const Vec3 &start, const Vec3 &end, const float t)
{
    return {lerp(start.x, end.x, t), lerp(start.y, end.y, t), lerp(start.z, end.z, t)};
}

std::optional<Vec3> intersect_plane(Vec3 point, Vec3 normal, Vec3 start, Vec3 end)
{
    Vec3 n = normalize(normal);
    Vec3 l = normalize(end - start);
    float a = dot((point - start), n);
    float b = dot(l, n);
    if (a == 0 || b == 0)
        return {};
    float d = a / b;
    return start + l * d;
}