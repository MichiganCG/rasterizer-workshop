#include "matrix.hpp"

Matrix4::Matrix4(const Matrix4 &other) : Matrix4()
{
    for (size_t i = 0; i < 16; ++i)
        data[i] = other.data[i];
}

Matrix4 &Matrix4::operator=(const Matrix4 &other)
{
    if (this == &other)
        return *this;

    for (size_t i = 0; i < 16; ++i)
        data[i] = other.data[i];

    return *this;
}

Matrix4 &Matrix4::operator+=(const Matrix4 &other)
{
    for (size_t i = 0; i < 16; ++i)
        data[i] += other.data[i];

    return *this;
}

Matrix4 operator+(Matrix4 lhs, const Matrix4 &rhs) { return (lhs += rhs); }

Matrix4 &Matrix4::operator-=(const Matrix4 &other)
{
    for (size_t i = 0; i < 16; ++i)
        data[i] -= other.data[i];

    return *this;
}

Matrix4 operator-(Matrix4 lhs, const Matrix4 &rhs) { return (lhs -= rhs); }

Matrix4 &Matrix4::operator*=(const Matrix4 &rhs)
{
    Matrix4 matrix;
    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            for (size_t k = 0; k < 4; ++k)
            {
                matrix.at(i, j) += (at(i, k) * rhs.at(k, j));
            }
        }
    }
    return (*this = matrix);
}

Matrix4 operator*(Matrix4 lhs, const Matrix4 &rhs) { return (lhs *= rhs); }

Vec3 operator*(const Matrix4 &lhs, const Vec3 &rhs)
{
    Vec3 vector;
    vector.x = rhs.x * lhs.at(0, 0) + rhs.y * lhs.at(0, 1) + rhs.z * lhs.at(0, 2) + rhs.w * lhs.at(0, 3);
    vector.y = rhs.x * lhs.at(1, 0) + rhs.y * lhs.at(1, 1) + rhs.z * lhs.at(1, 2) + rhs.w * lhs.at(1, 3);
    vector.z = rhs.x * lhs.at(2, 0) + rhs.y * lhs.at(2, 1) + rhs.z * lhs.at(2, 2) + rhs.w * lhs.at(2, 3);
    vector.w = rhs.x * lhs.at(3, 0) + rhs.y * lhs.at(3, 1) + rhs.z * lhs.at(3, 2) + rhs.w * lhs.at(3, 3);

    if (vector.w != 0) {
        vector /= vector.w;
        vector.w = 1;
    }

    return vector;
}

Vec3 operator*(const Vec3 &lhs, const Matrix4 &rhs)
{
    return rhs * lhs;
}

Matrix4 &Matrix4::operator*=(float rhs)
{
    for (size_t i = 0; i < 16; ++i)
    {
        data[i] *= rhs;
    }
    return *this;
}

Matrix4 operator*(Matrix4 lhs, float rhs) { return (lhs *= rhs); }

std::ostream &operator<<(std::ostream &os, const Matrix4 &rhs)
{
    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            os << rhs[i][j] << " ";
        }
    }
    return os;
}

Matrix4 look_at(const Vec3 &eye, const Vec3 &target, const Vec3 &up_dir)
{
    Vec3 forward = eye - target;
    forward = normalize(forward);

    Vec3 left = cross(up_dir, forward);
    left = normalize(left);

    Vec3 up = cross(forward, left);

    Matrix4 matrix;
    matrix.identity();

    matrix.at(0, 0) = left.x;
    matrix.at(0, 1) = left.y;
    matrix.at(0, 2) = left.z;
    matrix.at(1, 0) = up.x;
    matrix.at(1, 1) = up.y;
    matrix.at(1, 2) = up.z;
    matrix.at(2, 0) = forward.x;
    matrix.at(2, 1) = forward.y;
    matrix.at(2, 2) = forward.z;

    matrix.at(0, 3) = -left.x * eye.x - left.y * eye.y - left.z * eye.z;
    matrix.at(1, 3) = -up.x * eye.x - up.y * eye.y - up.z * eye.z;
    matrix.at(2, 3) = -forward.x * eye.x - forward.y * eye.y - forward.z * eye.z;

    return matrix;
}

Matrix4 &translate(Matrix4 &matrix, const Vec3 &vector)
{
    matrix.at(0, 3) = vector.x;
    matrix.at(1, 3) = vector.y;
    matrix.at(2, 3) = vector.z;
    return matrix;
}

Matrix4 translate(const Vec3 &vector)
{
    Matrix4 matrix;
    translate(matrix.identity(), vector);
    return matrix;
}

Matrix4 &rotate(Matrix4 &matrix, const Quaternion &q)
{
    float r = q.w, i = q.x, j = q.y, k = q.z;
    matrix.at(0, 0) = 1 - (2 * (j * j + k * k));
    matrix.at(0, 1) = 2 * (i * j - r * k);
    matrix.at(0, 2) = 2 * (i * k + r * j);
    matrix.at(1, 0) = 2 * (i * j + r * k);
    matrix.at(1, 1) = 1 - (2 * (i * i + k * k));
    matrix.at(1, 2) = 2 * (j * k - r * i);
    matrix.at(2, 0) = 2 * (i * k - r * j);
    matrix.at(2, 1) = 2 * (j * k + r * i);
    matrix.at(2, 2) = 1 - (2 * (i * i + j * j));
    matrix.at(3, 3) = 1;
    return matrix;
}

Matrix4 rotate(const Quaternion &q)
{
    Matrix4 matrix;
    rotate(matrix.identity(), q);
    return matrix;
}

Matrix4 quick_inverse(const Matrix4 &m)
{
    Matrix4 matrix;
    matrix.at(0, 0) = m.at(0, 0);
    matrix.at(0, 1) = m.at(1, 0);
    matrix.at(0, 2) = m.at(2, 0);
    matrix.at(1, 0) = m.at(0, 1);
    matrix.at(1, 1) = m.at(1, 1);
    matrix.at(1, 2) = m.at(2, 1);
    matrix.at(2, 0) = m.at(0, 2);
    matrix.at(2, 1) = m.at(1, 2);
    matrix.at(2, 2) = m.at(2, 2);
    matrix.at(3, 0) = -m.at(0, 0) * m.at(0, 3) - m.at(1, 0) * m.at(1, 3) - m.at(2, 0) * m.at(2, 3) + 0.0;
    matrix.at(3, 1) = -m.at(0, 1) * m.at(0, 3) - m.at(1, 1) * m.at(1, 3) - m.at(2, 1) * m.at(2, 3) + 0.0;
    matrix.at(3, 2) = -m.at(0, 2) * m.at(0, 3) - m.at(1, 2) * m.at(1, 3) - m.at(2, 2) * m.at(2, 3) + 0.0;
    matrix.at(3, 3) = 1;
    return matrix;
}

Matrix4 perspective_projection(float fov, float aspect_ratio, float near, float far)
{
    const float DEG2RAD = std::acos(-1.0f) / 180;

    float tangent = std::tan(fov / 2 * DEG2RAD);
    float right = near * tangent;
    float top = right / aspect_ratio;

    Matrix4 matrix;
    matrix.at(0, 0) = near / right;
    matrix.at(1, 1) = near / top;
    matrix.at(2, 2) = -(far + near) / (far - near);
    matrix.at(2, 3) = -(2 * far * near) / (far - near);
    matrix.at(3, 2) = -1;
    return matrix;
}

Matrix4 viewport(uint32_t width, uint32_t height)
{
    Matrix4 matrix;
    matrix.at(0, 0) = width / 2;
    matrix.at(1, 1) = height / 2;
    matrix.at(2, 2) = 1;
    matrix.at(0, 3) = (width - 1) / 2;
    matrix.at(1, 3) = (height - 1) / 2;
    matrix.at(3, 3) = 1;
    return matrix;
}