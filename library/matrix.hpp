#pragma once

#include "vectors.hpp"
#include "quaternion.hpp"

#include <cmath>
#include <ostream>

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
            if (col >= SIZE)
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
            if (col >= SIZE)
                throw std::runtime_error("Access outside of matrix columns.");
            return m.at(row, col);
        }
    };

public:
    static const size_t SIZE = 4, D_SIZE = 16;

    /**
     * Create an identity matrix with default ctor.
     */
    Matrix4() : data({0}) {}
    Matrix4(const Matrix4 &);

    Matrix4 &operator=(const Matrix4 &);

    float &at(size_t row, size_t col) { return data[row + col * SIZE]; }
    float at(size_t row, size_t col) const { return data[row + col * SIZE]; }

    Proxy operator[](size_t row)
    {
        if (row >= SIZE)
            throw std::runtime_error("Access outside of matrix rows.");
        return {*this, row};
    }

    ConstProxy operator[](size_t row) const
    {
        if (row >= SIZE)
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

    Matrix4 &operator+=(const Matrix4 &);
    Matrix4 &operator-=(const Matrix4 &);
    Matrix4 &operator*=(const Matrix4 &);
    Matrix4 &operator*=(float);

    friend std::ostream &operator<<(std::ostream &, const Matrix4 &);
};

Matrix4 operator+(Matrix4 lhs, const Matrix4 &rhs);
Matrix4 operator-(Matrix4 lhs, const Matrix4 &rhs);
Matrix4 operator*(Matrix4 lhs, const Matrix4 &rhs);
Matrix4 operator*(Matrix4 lhs, float rhs);

Vec3 operator*(const Matrix4 &, const Vec3 &);
Vec3 operator*(const Vec3 &, const Matrix4 &);

/**
 * Creates a rotation matrix that looks from the eye to the target.
 */
Matrix4 look_at(const Vec3 &eye, const Vec3 &target, const Vec3 &up_dir);

Matrix4 &translate(Matrix4 &, const Vec3 &);
Matrix4 translate(const Vec3 &);

Matrix4 &rotate(Matrix4 &, const Quaternion &);
/**
 * Converts the quaternion into rotation matrix form.
 * https://automaticaddison.com/how-to-convert-a-quaternion-to-a-rotation-matrix/
 */
Matrix4 rotate(const Quaternion &);

/**
 * Quickly inverts a translation and rotation matrix.
 * Transposes the rotation component and negates the translation component.
 * @returns The inverse of the rotation-translation matrix.
 */
Matrix4 quick_inverse(const Matrix4 &);

/**
 * Creates a symmetric frustum with horizontal FOV.
 * The field of view is measured in degrees. Aspect ratio is width / height.
 * Matrix is used to convert from view space to NDC space.
 * https://www.songho.ca/opengl/gl_projectionmatrix.html
 */
Matrix4 projection(float fov, float aspect_ratio, float near, float far);
Matrix4 viewport(uint32_t width, uint32_t height);