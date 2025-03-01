#include "vectors.hpp"
#include "library.hpp"

const Vec3 Vec3::RIGHT = Vec3(1, 0, 0, 0);
const Vec3 Vec3::UP = Vec3(0, 1, 0, 0);
const Vec3 Vec3::FORWARD = Vec3(0, 0, 1, 0);

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
    return *this;
}

Vec3 &Vec3::operator-=(const Vec3 &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Vec3 &Vec3::operator*=(const Vec3 &rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
}

Vec3 &Vec3::operator/=(const Vec3 &rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    return *this;
}

Vec3 &Vec3::operator*=(float rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}

Vec3 &Vec3::operator/=(float rhs)
{
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
}

Vec3::operator Vec2() const
{
    Vec2 v;
    v.u = x;
    v.v = y;
    v.w = z;
    return v;
}

Vec3 operator+(Vec3 lhs, const Vec3 &rhs) { return (lhs += rhs); }
Vec3 operator-(Vec3 lhs, const Vec3 &rhs) { return (lhs -= rhs); }
Vec3 operator*(Vec3 lhs, const Vec3 &rhs) { return (lhs *= rhs); }
Vec3 operator/(Vec3 lhs, const Vec3 &rhs) { return (lhs /= rhs); }
Vec3 operator*(Vec3 lhs, float rhs) { return (lhs *= rhs); }
Vec3 operator/(Vec3 lhs, float rhs) { return (lhs /= rhs); }

Vec3 operator+(const Vec3 &rhs)
{
    Vec3 vector;
    vector.x = +rhs.x;
    vector.y = +rhs.y;
    vector.z = +rhs.z;
    return vector;
}

Vec3 operator-(const Vec3 &rhs)
{
    Vec3 vector;
    vector.x = -rhs.x;
    vector.y = -rhs.y;
    vector.z = -rhs.z;
    return vector;
}

std::ostream &operator<<(std::ostream &os, const Vec3 &rhs)
{
    os << "( " << rhs.x << " " << rhs.y << " " << rhs.z << " " << rhs.w << " )";
    return os;
}

float dot(const Vec3 &lhs, const Vec3 &rhs) { return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z; }
float abs_dot(const Vec3 &lhs, const Vec3 &rhs) { return std::abs(dot(lhs, rhs)); }
float magnitude_squared(const Vec3 &vector) { return dot(vector, vector); }
float magnitude(const Vec3 &vector) { return std::sqrt(magnitude_squared(vector)); }

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
            lhs.x * rhs.y - lhs.y * rhs.x,
            0};
}

Vec3 project(const Vec3 &line, const Vec3 &vector)
{
    Vec3 n = normalize(line);
    float d = dot(n, vector);
    return n * d;
}

void orthonormal(Vec3 &normal, Vec3 &tangent)
{
    Vec3 n = project(normal, tangent);
    Vec3 t = normalize(tangent - n);
    normal = n;
    tangent = t;
}

std::optional<Vec3> intersect_plane(const Vec3 &point, const Vec3 &normal, const Vec3 &start, const Vec3 &end)
{
    Vec3 n = normalize(normal);
    Vec3 dir = end - start;
    float a = dot(n, point);
    float b = dot(n, dir);
    if (b == 0) // The line is parallel with the plane
        return {};

    float t = (a - dot(n, start)) / b;
    if (t >= 0 && t <= 1) // The intersection is in the line segment
        return start + dir * t;

    return {};
}

Vec2 &Vec2::operator=(const Vec2 &other)
{
    if (this == &other)
        return *this;
    u = other.u;
    v = other.v;
    w = other.w;
    return *this;
}

Vec2 &Vec2::operator+=(const Vec2 &rhs)
{
    u += rhs.u;
    v += rhs.v;
    w += rhs.w;
    return *this;
}

Vec2 operator+(Vec2 lhs, const Vec2 &rhs) { return (lhs += rhs); }

Vec2 &Vec2::operator-=(const Vec2 &rhs)
{
    u -= rhs.u;
    v -= rhs.v;
    w -= rhs.w;
    return *this;
}

Vec2 operator-(Vec2 lhs, const Vec2 &rhs) { return (lhs -= rhs); }

std::ostream &operator<<(std::ostream &os, const Vec2 &rhs)
{
    os << "( " << rhs.u << ", " << rhs.v << " " << rhs.w << " )";
    return os;
}

float dot(const Vec2 &lhs, const Vec2 &rhs) { return lhs.u * rhs.u + lhs.v * rhs.v; }
float abs_dot(const Vec2 &lhs, const Vec2 &rhs) { return std::abs(dot(lhs, rhs)); }
float magnitude_squared(const Vec2 &vector) { return dot(vector, vector); }
float magnitude(const Vec2 &vector) { return std::sqrt(magnitude_squared(vector)); }
