#include "vectors.hpp"
#include "library.hpp"

Vec3 &Vec3::operator=(const Vec3 &other)
{
    if (this == &other)
        return *this;
    x = other.x;
    y = other.y;
    z = other.z;
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
    os << "( " << rhs.x << " " << rhs.y << " " << rhs.z << " )";
    return os;
}

Vec3 normalize(const Vec3 &input)
{
    float squared = magnitude_squared(input);
    if (almost_zero(squared))
        return Vec4();
    return input / std::sqrt(squared);
}

Vec3 cross(const Vec3 &lhs, const Vec3 &rhs)
{
    return {lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x};
}


float dot(const Vec3 &lhs, const Vec3 &rhs) { return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z; }
float abs_dot(const Vec3 &lhs, const Vec3 &rhs) { return std::abs(dot(lhs, rhs)); }
float magnitude_squared(const Vec3 &input) { return dot(input, input); }
float magnitude(const Vec3 &input) { return std::sqrt(magnitude_squared(input)); }

const Vec4 Vec4::RIGHT = Vec4(1, 0, 0, 0);
const Vec4 Vec4::UP = Vec4(0, 1, 0, 0);
const Vec4 Vec4::FORWARD = Vec4(0, 0, 1, 0);

Vec4 &Vec4::operator=(const Vec4 &other)
{
    if (this == &other)
        return *this;
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    return *this;
}

Vec4 &Vec4::operator+=(const Vec4 &rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}

Vec4 &Vec4::operator-=(const Vec4 &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}

Vec4 &Vec4::operator*=(const Vec4 &rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;
    return *this;
}

Vec4 &Vec4::operator/=(const Vec4 &rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;
    return *this;
}

Vec4 &Vec4::operator*=(float rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
    w *= rhs;
    return *this;
}

Vec4 &Vec4::operator/=(float rhs)
{
    x /= rhs;
    y /= rhs;
    z /= rhs;
    w /= rhs;
    return *this;
}

Vec4::operator Vec3() const
{
    Vec3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

Vec4 operator+(Vec4 lhs, const Vec4 &rhs) { return (lhs += rhs); }
Vec4 operator-(Vec4 lhs, const Vec4 &rhs) { return (lhs -= rhs); }
Vec4 operator*(Vec4 lhs, const Vec4 &rhs) { return (lhs *= rhs); }
Vec4 operator/(Vec4 lhs, const Vec4 &rhs) { return (lhs /= rhs); }
Vec4 operator*(Vec4 lhs, float rhs) { return (lhs *= rhs); }
Vec4 operator/(Vec4 lhs, float rhs) { return (lhs /= rhs); }

Vec4 operator+(const Vec4 &rhs)
{
    Vec4 vector;
    vector.x = +rhs.x;
    vector.y = +rhs.y;
    vector.z = +rhs.z;
    vector.w = +rhs.w;
    return vector;
}

Vec4 operator-(const Vec4 &rhs)
{
    Vec4 vector;
    vector.x = -rhs.x;
    vector.y = -rhs.y;
    vector.z = -rhs.z;
    vector.w = -rhs.w;
    return vector;
}

std::ostream &operator<<(std::ostream &os, const Vec4 &rhs)
{
    os << "( " << rhs.x << " " << rhs.y << " " << rhs.z << " " << rhs.w << " )";
    return os;
}

float dot(const Vec4 &lhs, const Vec4 &rhs) { return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w; }
float abs_dot(const Vec4 &lhs, const Vec4 &rhs) { return std::abs(dot(lhs, rhs)); }
float magnitude_squared(const Vec4 &input) { return dot(input, input); }
float magnitude(const Vec4 &input) { return std::sqrt(magnitude_squared(input)); }

Vec4 normalize(const Vec4 &input)
{
    float squared = magnitude_squared(input);
    if (almost_zero(squared))
        return Vec4();
    return input / std::sqrt(squared);
}

Vec4 cross(const Vec4 &lhs, const Vec4 &rhs)
{
    return {lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x,
            0};
}

Vec4 project(const Vec4 &line, const Vec4 &vector)
{
    Vec4 n = normalize(line);
    float d = dot(n, vector);
    return n * d;
}

void orthonormal(Vec4 &normal, Vec4 &tangent)
{
    Vec4 n = project(normal, tangent);
    Vec4 t = normalize(tangent - n);
    normal = n;
    tangent = t;
}

std::optional<Vec4> intersect_plane(const Vec4 &point, const Vec4 &normal, const Vec4 &start, const Vec4 &end)
{
    Vec4 norm = normalize(normal);
    Vec4 ray = end - start;

    Vec4 diff = start - point;
    float a = dot(diff, norm);
    float b = dot(ray, norm);
    if (std::abs(b) < std::numeric_limits<float>::epsilon())
        return {};

    float t = a / b;
    return start - ray * t;
}
