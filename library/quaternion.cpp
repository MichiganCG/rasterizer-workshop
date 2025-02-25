#include "quaternion.hpp"
#include "library.hpp"

#define epsilon 8E-7f

Quaternion::Quaternion() : r(0.0f), i(0.0f), j(0.0f), k(1.0f) {}

Quaternion::Quaternion(float r, float i, float j, float k) : r(r), i(i), j(j), k(k) {}

Quaternion::Quaternion(const Quaternion &other) : r(other.r), i(other.i), j(other.j), k(other.k) {}

Quaternion &Quaternion::operator=(const Quaternion &other)
{
    if (this == &other)
        return *this;
    r = other.r;
    i = other.i;
    j = other.j;
    k = other.k;
    return *this;
}

/**
 * Applies the quaternion to the vector (0, 0, 1).
 */
Vec3 Quaternion::forward()
{
    return {
        2 * (i * k + r * j),
        2 * (j * k - r * i),
        1 - (2 * (i * i + j * j))};
}

/**
 * Applies the quaternion to the vector (0, 1, 0).
 */
Vec3 Quaternion::up()
{
    return {
        2 * (i * j - r * k),
        1 - (2 * (i * i + k * k)),
        2 * (j * k + r * i)};
}

/**
 * Applies the quaternion to the vector (1, 0, 0).
 */
Vec3 Quaternion::left()
{
    return {
        1 - (2 * (j * j + k * k)),
        2 * (i * j + r * k),
        2 * (i * k - r * j)};
}

/**
 * Returns the imaginary component, also known as the bivector.
 */
Vec3 Quaternion::bivector()
{
    return {i, j, k};
}

Quaternion &Quaternion::operator*=(const Quaternion &rhs)
{
    Quaternion quaternion;
    quaternion.r = (r * rhs.r) - (i * rhs.i) - (j * rhs.j) - (k * rhs.k);
    quaternion.i = (r * rhs.i) + (i * rhs.r) + (j * rhs.k) - (k * rhs.j);
    quaternion.j = (r * rhs.j) - (i * rhs.k) + (j * rhs.r) + (k * rhs.i);
    quaternion.k = (r * rhs.k) + (i * rhs.j) - (j * rhs.i) + (k * rhs.r);
    return (*this = normalize(quaternion));
}

Quaternion &Quaternion::operator*=(float rhs)
{
    r *= rhs;
    i *= rhs;
    j *= rhs;
    k *= rhs;
    return *this;
}

Quaternion &Quaternion::operator/=(float rhs)
{
    r /= rhs;
    i /= rhs;
    j /= rhs;
    k /= rhs;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const Quaternion &rhs)
{
    os << rhs.r << " " << rhs.i << " " << rhs.j << " " << rhs.k << std::endl;
    return os;
}

float dot(const Quaternion &lhs, const Quaternion &rhs)
{
    return lhs.r * rhs.r + lhs.i * rhs.i + lhs.j * rhs.j + lhs.k * rhs.k;
}

float magnitude_squared(const Quaternion &q)
{
    return dot(q, q);
}

float magnitude(const Quaternion &q)
{
    return std::sqrt(magnitude_squared(q));
}

/**
 * https://stackoverflow.com/questions/11667783/quaternion-and-normalization
 */
Quaternion normalize(const Quaternion &q)
{
    float qmagsq = magnitude_squared(q);
    if (std::abs(1.0f - qmagsq) < 2.107342E-08)
        return q * (2.0f / (1.0f + qmagsq));
    return q * (1.0f / std::sqrt(qmagsq));
}

Quaternion conjugate(const Quaternion &q)
{
    return {q.r, -q.i, -q.j, -q.k};
}

void Quaternion::get_axis_angle(Vec3 &axis, float &angle)
{
    Vec3 biv = bivector();
    float s = magnitude(biv);
    axis = biv / s;

    angle = 2 * std::atan2(s, r);
}

Quaternion local_rotation(const Vec3 &axis, float angle)
{
    float half_angle = angle * 0.5;
    return {
        std::cos(half_angle),
        axis.x * std::sin(half_angle),
        axis.y * std::sin(half_angle),
        axis.z * std::sin(half_angle)};
}

void Quaternion::rotate(const Vec3 &axis, float angle)
{
    Quaternion local = local_rotation(normalize(axis), angle);
    *this *= local;
}