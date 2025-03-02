#pragma once

#include "library.hpp"

#include <cmath>
#include <optional>
#include <ostream>

struct Vec2
{
	float u, v, w;

	/**
	 * Default constructor.
	 * Initializes a point at the origin.
	 */
	Vec2() : u(0), v(0), w(1) {}

	/**
	 * Constructs a point with the given components.
	 */
	Vec2(float u, float v) : u(u), v(v), w(1) {}

	/**
	 * Constructs a point with the given components.
	 */
	Vec2(uint32_t u, uint32_t v) : u((float)u), v((float)v), w(1) {}

	/**
	 * Constructs a point with the given components.
	 */
	Vec2(float u, float v, float w) : u(u), v(v), w(w) {}

	/**
	 * Copy constructor.
	 */
	Vec2(const Vec2 &other) : u(other.u), v(other.v), w(other.w) {}

	Vec2 &operator=(const Vec2 &rhs);
	Vec2 &operator+=(const Vec2 &rhs);
	Vec2 &operator-=(const Vec2 &rhs);

	friend std::ostream &operator<<(std::ostream &os, const Vec2 &rhs);
};

Vec2 operator+(Vec2 lhs, const Vec2 &rhs);
Vec2 operator-(Vec2 lhs, const Vec2 &rhs);

/**
 * Computes the dot product of two (2) vectors.
 * @returns The dot product
 */
float dot(const Vec2 &lhs, const Vec2 &rhs);

/**
 * Computes the absolute value of the dot product of two (2) vectors.
 * @returns The absolute value of the dot product
 */
float abs_dot(const Vec2 &lhs, const Vec2 &rhs);

/**
 * Computes the square of the magnitude of the given vector.
 * @returns The square of the magnitude
 */
float magnitude_squared(const Vec2 &input);

/**
 * Computes the magnitude of the given vector.
 * @returns The magnitude
 */
float magnitude(const Vec2 &input);

/**
 * An implementation of 3-dimensional vectors and points.
 */
struct Vec3
{
	float x, y, z, w;

	/**
	 * Default constructor.
	 * Initializes a point at the origin.
	 */
	Vec3() : x(0), y(0), z(0), w(1) {}

	/**
	 * Constructs a point with the given components.
	 */
	Vec3(float x, float y, float z) : x(x), y(y), z(z), w(1) {}

	/**
	 * Constructs a vector with the given components.
	 */
	Vec3(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	/**
	 * Copy constructor.
	 * Constructs a copy of the given vector.
	 */
	Vec3(const Vec3 &other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

	Vec3 &operator=(const Vec3 &rhs);
	Vec3 &operator+=(const Vec3 &rhs);
	Vec3 &operator-=(const Vec3 &rhs);
	Vec3 &operator*=(const Vec3 &rhs);
	Vec3 &operator/=(const Vec3 &rhs);
	Vec3 &operator*=(float rhs);
	Vec3 &operator/=(float rhs);

	friend std::ostream &operator<<(std::ostream &os, const Vec3 &rhs);

	static const Vec3 RIGHT;   // The vector `<1, 0, 0>`
	static const Vec3 UP;	   // The vector `<0, 1, 0>`
	static const Vec3 FORWARD; // The vector `<0, 0, 1>`

	operator Vec2() const;
};

Vec3 operator+(Vec3 lhs, const Vec3 &rhs);
Vec3 operator-(Vec3 lhs, const Vec3 &rhs);
Vec3 operator*(Vec3 lhs, const Vec3 &rhs);
Vec3 operator/(Vec3 lhs, const Vec3 &rhs);
Vec3 operator*(Vec3 lhs, float rhs);
Vec3 operator/(Vec3 lhs, float rhs);
Vec3 operator+(const Vec3 &rhs);
Vec3 operator-(const Vec3 &rhs);

/**
 * Computes the dot product of two (2) vectors.
 * @returns The dot product
 */
float dot(const Vec3 &lhs, const Vec3 &rhs);

/**
 * Computes the absolute value of the dot product of two (2) vectors.
 * @returns The absolute value of the dot product
 */
float abs_dot(const Vec3 &lhs, const Vec3 &rhs);

/**
 * Computes the square of the magnitude of the given vector.
 * @returns The square of the magnitude
 */
float magnitude_squared(const Vec3 &input);

/**
 * Computes the magnitude of the given vector.
 * @returns The magnitude
 */
float magnitude(const Vec3 &input);

/**
 * Normalizes the given vector to be of length one (1).
 * Returns the zero vector if the original vector is very close to zero.
 * @returns A unit vector with the same direction as the given vector
 */
Vec3 normalize(const Vec3 &input);

/**
 * Computes the cross product of two (2) vectors.
 * @returns The cross product
 */
Vec3 cross(const Vec3 &lhs, const Vec3 &rhs);

/**
 * Projects the given vector onto the given line.
 * @returns The projection
 */
Vec3 project(const Vec3 &line, const Vec3 &vector);

/**
 * Makes the normal vector orthogonal to the tangent vector.
 */
void orthonormal(Vec3 &normal, Vec3 &tangent);

/**
 * Computes the intersection between the given line segment and plane.
 * @param point A point on the plane
 * @param normal A vector normal to the plane
 * @param start The start of the line segment
 * @param end The end of the line segment
 * @returns Optionally the point of intersection
 */
std::optional<Vec3> intersect_plane(const Vec3 &point, const Vec3 &normal, const Vec3 &start, const Vec3 &end);
