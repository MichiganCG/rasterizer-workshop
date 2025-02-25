#pragma once

#include <cmath>
#include <optional>
#include <ostream>

struct Vec2
{
	float u, v, w;

	Vec2() : u(0), v(0), w(1) {}
	Vec2(float u, float v) : u(u), v(v), w(1) {}
	Vec2(float value) : u(value), v(value), w(1) {}
	Vec2(const Vec2 &other) : u(other.u), v(other.v), w(1) {}
	Vec2 &operator=(const Vec2 &);
};

struct Vec3
{
	float x, y, z, w;

	Vec3() : x(0), y(0), z(0), w(1) {}
	Vec3(float x, float y, float z) : x(x), y(y), z(z), w(1) {}
	Vec3(float value) : x(value), y(value), z(value), w(1) {}
	Vec3(const Vec3 &other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

	Vec3 &operator=(const Vec3 &);
	Vec3 &operator+=(const Vec3 &);
	Vec3 &operator-=(const Vec3 &);
	Vec3 &operator*=(const Vec3 &);
	Vec3 &operator/=(const Vec3 &);
	Vec3 &operator*=(float);
	Vec3 &operator/=(float);

	friend std::ostream &operator<<(std::ostream &, const Vec3 &);
};

inline Vec3 operator+(Vec3 lhs, const Vec3 &rhs) { return (lhs += rhs); }
inline Vec3 operator-(Vec3 lhs, const Vec3 &rhs) { return (lhs -= rhs); }
inline Vec3 operator*(Vec3 lhs, const Vec3 &rhs) { return (lhs *= rhs); }
inline Vec3 operator/(Vec3 lhs, const Vec3 &rhs) { return (lhs /= rhs); }
inline Vec3 operator*(Vec3 lhs, float rhs) { return (lhs *= rhs); }
inline Vec3 operator/(Vec3 lhs, float rhs) { return (lhs /= rhs); }
Vec3 operator+(const Vec3 &);
Vec3 operator-(const Vec3 &);

inline float dot(const Vec3 &lhs, const Vec3 &rhs) { return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z; }
inline float abs_dot(const Vec3 &lhs, const Vec3 &rhs) { return std::abs(dot(lhs, rhs)); }
inline float magnitude_squared(const Vec3 &vector) { return dot(vector, vector); }
inline float magnitude(const Vec3 &vector) { return std::sqrt(magnitude_squared(vector)); }
inline float lerp(float a, float b, float t) { return (1.0f - t) * a + b * t; }

/**
 * Normalizes a vector.
 * Returns the zero vector if the original vector is very close to zero.
 * @return A unit vector with the same direction as the original vector.
 * @note Only considers the x, y, z components.
 */
Vec3 normalize(const Vec3 &);

/**
 * Computes the cross product of two vectors.
 */
Vec3 cross(const Vec3 &, const Vec3 &);

Vec3 lerp(const Vec3 &, const Vec3 &, float);

/**
 * Computes the point where the line segment and plane intersect.
 * @return The point of intersection. If there is no intersection, returns nothing.
 */
std::optional<Vec3> intersect_plane(Vec3, Vec3, Vec3, Vec3);
