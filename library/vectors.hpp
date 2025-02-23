#pragma once

#include "matrix.hpp"

#include <cmath>
#include <tuple>
#include <string>
#include <vector>
#include <optional>

class Vec2
{
public:
	float u, v, w;

	Vec2();
	Vec2(float, float);
	Vec2(float);
	Vec2(const Vec2 &);
	Vec2 &operator=(const Vec2 &);
};

class Vec3
{
public:
	float x, y, z, w;

	Vec3();
	Vec3(float, float, float);
	Vec3(float);
	Vec3(const Vec3 &);

	Vec3 &operator=(const Vec3 &);
	Vec3 &operator+=(const Vec3 &);
	Vec3 &operator-=(const Vec3 &);
	Vec3 &operator*=(const Vec3 &);
	Vec3 &operator/=(const Vec3 &);
	Vec3 &operator*=(const Matrix4 &);
	Vec3 &operator*=(const float);
	Vec3 &operator/=(const float);

	friend std::ostream &operator<<(std::ostream &, const Vec3 &);
};

inline Vec3 operator+(Vec3 lhs, const Vec3 &rhs) { return (lhs += rhs); }
inline Vec3 operator-(Vec3 lhs, const Vec3 &rhs) { return (lhs -= rhs); }
inline Vec3 operator*(Vec3 lhs, const Vec3 &rhs) { return (lhs *= rhs); }
inline Vec3 operator/(Vec3 lhs, const Vec3 &rhs) { return (lhs /= rhs); }
inline Vec3 operator*(Vec3 lhs, const Matrix4 &rhs) { return (lhs *= rhs); }
inline Vec3 operator*(const Matrix4 &lhs, Vec3 rhs) { return (rhs *= lhs); }
inline Vec3 operator*(Vec3 lhs, const float rhs) { return (lhs *= rhs); }
inline Vec3 operator/(Vec3 lhs, const float rhs) { return (lhs /= rhs); }
inline Vec3 operator+(const Vec3 &v)
{
	Vec3 temp;
	temp.x = +v.x;
	temp.y = +v.y;
	temp.z = +v.z;
	temp.w = +v.w;
	return temp;
}
inline Vec3 operator-(const Vec3 &v)
{
	Vec3 temp;
	temp.x = -v.x;
	temp.y = -v.y;
	temp.z = -v.z;
	temp.w = -v.w;
	return temp;
}

inline float dot(const Vec3 &, const Vec3 &);
inline float abs_dot(const Vec3 &, const Vec3 &);
inline float magnitude_squared(const Vec3 &);
inline float magnitude(const Vec3 &);
inline float lerp(const float a, const float b, const float t);

/**
 * Normalizes a vector.
 * Returns the zero vector if the original vector is very close to zero.
 * @return A unit vector with the same direction as the original vector.
 * @note Only considers the x, y, and z components.
 */
Vec3 normalize(const Vec3 &);

/**
 * Computes the cross product of two vectors.
 */
Vec3 cross(const Vec3 &, const Vec3 &);

Vec3 lerp(const Vec3 &, const Vec3 &, const float);

/**
 * Computes the point where the line segment and plane intersect.
 * @return The point of intersection. If there is no intersection, returns nothing.
 */
std::optional<Vec3> intersect_plane(Vec3, Vec3, Vec3, Vec3);
