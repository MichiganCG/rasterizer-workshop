#include "vectors.hpp"
#include "library.hpp"

Vec2 &Vec2::operator=(const Vec2 &other)
{
    if (this == &other)
        return *this;
    u = other.u;
    v = other.v;
    w = other.w;
    return *this;
}

const Vec3 Vec3::RIGHT = Vec3(1, 0, 0);
const Vec3 Vec3::UP = Vec3(0, 1, 0);
const Vec3 Vec3::FORWARD = Vec3(0, 0, 1);

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

Vec3 operator+(Vec3 lhs, const Vec3 &rhs) { return (lhs += rhs); }

Vec3 &Vec3::operator-=(const Vec3 &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}

Vec3 operator-(Vec3 lhs, const Vec3 &rhs) { return (lhs -= rhs); }

Vec3 &Vec3::operator*=(const Vec3 &rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;
    return *this;
}

Vec3 operator*(Vec3 lhs, const Vec3 &rhs) { return (lhs *= rhs); }

Vec3 &Vec3::operator/=(const Vec3 &rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;
    return *this;
}

Vec3 operator/(Vec3 lhs, const Vec3 &rhs) { return (lhs /= rhs); }

Vec3 &Vec3::operator*=(float rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
    w *= rhs;
    return *this;
}

Vec3 operator*(Vec3 lhs, float rhs) { return (lhs *= rhs); }

Vec3 &Vec3::operator/=(float rhs)
{
    x /= rhs;
    y /= rhs;
    z /= rhs;
    w /= rhs;
    return *this;
}

Vec3 operator/(Vec3 lhs, float rhs) { return (lhs /= rhs); }

Vec3 operator+(const Vec3 &rhs)
{
    Vec3 vector;
    vector.x = +rhs.x;
    vector.y = +rhs.y;
    vector.z = +rhs.z;
    vector.w = +rhs.w;
    return vector;
}

Vec3 operator-(const Vec3 &rhs)
{
    Vec3 vector;
    vector.x = -rhs.x;
    vector.y = -rhs.y;
    vector.z = -rhs.z;
    vector.w = -rhs.w;
    return vector;
}

std::ostream &operator<<(std::ostream &os, const Vec3 &rhs)
{
    os << rhs.x << " " << rhs.y << " " << rhs.z << " " << rhs.w << std::endl;
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
            lhs.x * rhs.y - lhs.y * rhs.x};
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