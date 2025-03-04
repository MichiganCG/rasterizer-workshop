#pragma once

#include "library.hpp"

#include <cmath>
#include <optional>
#include <ostream>

struct Vec3
{
	float x, y, z;

	/**
	 * Default constructor.
	 * Initializes a point at the origin.
	 */
	Vec3() : x(0), y(0), z(0) {}

	/**
	 * Constructs a point with the given components.
	 */
	Vec3(float value) : x(value), y(value), z(value) {}

	/**
	 * Constructs a point with the given components.
	 */
	Vec3(float u, float v) : x(u), y(v), z(0) {}

	/**
	 * Constructs a point with the given components.
	 */
	Vec3(uint32_t u, uint32_t v) : x((float)u), y((float)v), z(0) {}

	/**
	 * Constructs a point with the given components.
	 */
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

	/**
	 * Copy constructor.
	 */
	Vec3(const Vec3 &other) : x(other.x), y(other.y), z(other.z) {}

	Vec3 &operator=(const Vec3 &rhs);
	Vec3 &operator+=(const Vec3 &rhs);
	Vec3 &operator-=(const Vec3 &rhs);
	Vec3 &operator*=(const Vec3 &rhs);
	Vec3 &operator/=(const Vec3 &rhs);
	Vec3 &operator*=(float rhs);
	Vec3 &operator/=(float rhs);

	friend std::ostream &operator<<(std::ostream &os, const Vec3 &rhs);
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
 * An implementation of 3-dimensional vectors and points.
 */
struct Vec4
{
	float x, y, z, w;

	/**
	 * Default constructor.
	 * Initializes a point at the origin.
	 */
	Vec4() : x(0), y(0), z(0), w(1) {}

	/**
	 * Constructs a point with the given components.
	 */
	Vec4(float x, float y, float z) : x(x), y(y), z(z), w(1) {}

	/**
	 * Constructs a vector with the given components.
	 */
	Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	/**
	 * Copy constructor.
	 * Constructs a copy of the given vector.
	 */
	Vec4(const Vec4 &other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

	Vec4 &operator=(const Vec4 &rhs);
	Vec4 &operator+=(const Vec4 &rhs);
	Vec4 &operator-=(const Vec4 &rhs);
	Vec4 &operator*=(const Vec4 &rhs);
	Vec4 &operator/=(const Vec4 &rhs);
	Vec4 &operator*=(float rhs);
	Vec4 &operator/=(float rhs);

	friend std::ostream &operator<<(std::ostream &os, const Vec4 &rhs);

	static const Vec4 ZERO; // The vector `<0, 0, 0, 0>`
	static const Vec4 RIGHT;   // The vector `<1, 0, 0, 0>`
	static const Vec4 UP;	   // The vector `<0, 1, 0, 0>`
	static const Vec4 FORWARD; // The vector `<0, 0, 1, 0>`

	operator Vec3() const;
};

Vec4 operator+(Vec4 lhs, const Vec4 &rhs);
Vec4 operator-(Vec4 lhs, const Vec4 &rhs);
Vec4 operator*(Vec4 lhs, const Vec4 &rhs);
Vec4 operator/(Vec4 lhs, const Vec4 &rhs);
Vec4 operator*(Vec4 lhs, float rhs);
Vec4 operator/(Vec4 lhs, float rhs);
Vec4 operator+(const Vec4 &rhs);
Vec4 operator-(const Vec4 &rhs);

/**
 * Computes the dot product of two (2) vectors.
 * @returns The dot product
 */
float dot(const Vec4 &lhs, const Vec4 &rhs);

/**
 * Computes the absolute value of the dot product of two (2) vectors.
 * @returns The absolute value of the dot product
 */
float abs_dot(const Vec4 &lhs, const Vec4 &rhs);

/**
 * Computes the square of the magnitude of the given vector.
 * @returns The square of the magnitude
 */
float magnitude_squared(const Vec4 &input);

/**
 * Computes the magnitude of the given vector.
 * @returns The magnitude
 */
float magnitude(const Vec4 &input);

/**
 * Normalizes the given vector to be of length one (1).
 * Returns the zero vector if the original vector is very close to zero.
 * @returns A unit vector with the same direction as the given vector
 */
Vec4 normalize(const Vec4 &input);

/**
 * Computes the cross product of two (2) vectors.
 * @returns The cross product
 */
Vec4 cross(const Vec4 &lhs, const Vec4 &rhs);

/**
 * Projects the given vector onto the given line.
 * @returns The projection
 */
Vec4 project(const Vec4 &line, const Vec4 &vector);

/**
 * Makes the normal vector orthogonal to the tangent vector.
 */
void orthonormal(Vec4 &normal, Vec4 &tangent);

/**
 * Computes the intersection between the given line segment and plane.
 * @param point A point on the plane
 * @param normal A vector normal to the plane
 * @param start The start of the line segment
 * @param end The end of the line segment
 * @returns Optionally the point of intersection
 */
std::optional<Vec4> intersect_plane(const Vec4 &point, const Vec4 &normal, const Vec4 &start, const Vec4 &end);
