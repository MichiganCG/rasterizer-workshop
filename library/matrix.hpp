#pragma once

#include "vectors.hpp"
#include "quaternion.hpp"

#include <cmath>
#include <ostream>

/**
 * Implementation of 4-by-4 matrices.
 */
class Matrix4
{
private:
    float data[16];

    class Proxy
    {
        friend class Matrix4;
        size_t row;
        Matrix4 &m;
        Proxy(Matrix4 &m, size_t row) : row(row), m(m) {}

    public:
        float &operator[](size_t col)
        {
            if (col >= 4)
                throw std::runtime_error("Access outside of matrix columns.");
            return m.at(row, col);
        }
    };

    class ConstProxy
    {
        friend class Matrix4;
        size_t row;
        const Matrix4 &m;
        ConstProxy(const Matrix4 &m, size_t row) : row(row), m(m) {}

    public:
        float operator[](size_t col) const
        {
            if (col >= 4)
                throw std::runtime_error("Access outside of matrix columns.");
            return m.at(row, col);
        }
    };

public:
    /**
     * Default constructor.
     * Initializes an empty matrix.
     */
    Matrix4() : data({0}) {}

    /**
     * Copy constructor.
     * Initializes a matrix with the same values.
     */
    Matrix4(const Matrix4 &);

    Matrix4 &operator=(const Matrix4 &);

    float &at(size_t row, size_t col) { return data[row + col * 4]; }
    float at(size_t row, size_t col) const { return data[row + col * 4]; }

    Proxy operator[](size_t row)
    {
        if (row >= 4)
            throw std::runtime_error("Access outside of matrix rows.");
        return {*this, row};
    }

    ConstProxy operator[](size_t row) const
    {
        if (row >= 4)
            throw std::runtime_error("Access outside of matrix rows.");
        return {*this, row};
    }

    // Make this matrix an identity matrix.
    Matrix4 &identity()
    {
        Matrix4 matrix;
        matrix.at(0, 0) = 1;
        matrix.at(1, 1) = 1;
        matrix.at(2, 2) = 1;
        matrix.at(3, 3) = 1;
        return (*this = matrix);
    }

    Matrix4 &operator+=(const Matrix4 &rhs);
    Matrix4 &operator-=(const Matrix4 &rhs);
    Matrix4 &operator*=(const Matrix4 &rhs);
    Matrix4 &operator*=(float);

    friend std::ostream &operator<<(std::ostream &os, const Matrix4 &rhs);
};

Matrix4 operator+(Matrix4 lhs, const Matrix4 &rhs);
Matrix4 operator-(Matrix4 lhs, const Matrix4 &rhs);
Matrix4 operator*(Matrix4 lhs, const Matrix4 &rhs);
Matrix4 operator*(Matrix4 lhs, float rhs);

Vec3 operator*(const Matrix4 &lhs, const Vec3 &rhs);
Vec3 operator*(const Vec3 &lhs, const Matrix4 &rhs);

/**
 * Creates a rotation matrix that looks from the eye to the target.
 */
Matrix4 look_at(const Vec3 &eye, const Vec3 &target, const Vec3 &up_dir);

Matrix4 &translate(Matrix4 &matrix, const Vec3 &translation);
Matrix4 translate(const Vec3 &translation);

/**
 * Converts the given quaternion into rotation matrix form.
 * https://automaticaddison.com/how-to-convert-a-quaternion-to-a-rotation-matrix/
 */
Matrix4 &rotate(Matrix4 &matrix, const Quaternion &rotation);
Matrix4 rotate(const Quaternion &rotation);

/**
 * Quickly inverts a translation and rotation matrix.
 * Because these matrices have a specific format, we can simply transpose the
 * rotation component and negate the translation component to find the inverse.
 * @returns The inverse of the rotation-translation matrix.
 */
Matrix4 quick_inverse(const Matrix4 &input);

/**
 * Creates a symmetric frustum using horizontal FOV.
 *
 * This matrix is used to convert from view space to clip space.
 * https://www.songho.ca/opengl/gl_projectionmatrix.html
 * @param fov          The horizontal field of view measured in degrees.
 * @param aspect_ratio Defined as `width / height`.
 * @param near         The distance to the 'near' clipping plane through the negative z-axis.
 * @param far          The distance to the 'far' clipping plane through the negative z-axis.
 */
Matrix4 perspective_projection(float fov, float aspect_ratio, float near, float far);

/**
 * Creates a matrix that transforms from NDC space to screen space.
 */
Matrix4 viewport(uint32_t width, uint32_t height);