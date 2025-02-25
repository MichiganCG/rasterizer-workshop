#include "matrix.hpp"

Matrix4::Matrix4(const Matrix4 &other) : Matrix4()
{
    for (size_t i = 0; i < D_SIZE; ++i)
        data[i] = other.data[i];
}

Matrix4 &Matrix4::operator=(const Matrix4 &other)
{
    if (this == &other)
        return *this;

    for (size_t i = 0; i < D_SIZE; ++i)
        data[i] = other.data[i];

    return *this;
}

Matrix4 &Matrix4::operator+=(const Matrix4 &other)
{
    for (size_t i = 0; i < D_SIZE; ++i)
        data[i] += other.data[i];

    return *this;
}

Matrix4 &Matrix4::operator-=(const Matrix4 &other)
{
    for (size_t i = 0; i < D_SIZE; ++i)
        data[i] -= other.data[i];

    return *this;
}

Matrix4 &Matrix4::operator*=(const Matrix4 &rhs)
{
    Matrix4 matrix;
    for (size_t i = 0; i < Matrix4::SIZE; ++i)
    {
        for (size_t j = 0; j < Matrix4::SIZE; ++j)
        {
            for (size_t k = 0; k < Matrix4::SIZE; ++k)
            {
                matrix.at(i, j) += (at(i, k) * rhs.at(k, j));
            }
        }
    }
    return (*this = matrix);
}

Vec3 operator*(const Matrix4 &lhs, const Vec3 &rhs)
{
    Vec3 vector;
    vector.x = rhs.x * lhs.at(0, 0) + rhs.y * lhs.at(0, 1) + rhs.z * lhs.at(0, 2) + rhs.w * lhs.at(0, 3);
    vector.y = rhs.x * lhs.at(1, 0) + rhs.y * lhs.at(1, 1) + rhs.z * lhs.at(1, 2) + rhs.w * lhs.at(1, 3);
    vector.z = rhs.x * lhs.at(2, 0) + rhs.y * lhs.at(2, 1) + rhs.z * lhs.at(2, 2) + rhs.w * lhs.at(2, 3);
    vector.w = rhs.x * lhs.at(3, 0) + rhs.y * lhs.at(3, 1) + rhs.z * lhs.at(3, 2) + rhs.w * lhs.at(3, 3);
    return vector;
}

Matrix4 &Matrix4::operator*=(float rhs)
{
    for (size_t i = 0; i < Matrix4::D_SIZE; ++i)
    {
        data[i] *= rhs;
    }
    return *this;
}

std::ostream &operator<<(std::ostream &os, const Matrix4 &rhs)
{
    for (size_t i = 0; i < Matrix4::SIZE; ++i)
    {
        for (size_t j = 0; j < Matrix4::SIZE; ++j)
        {
            os << rhs[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
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
    matrix.identity();
    translate(matrix, vector);
    return matrix;
}

/**
 * Converts the quaternion into rotation matrix form.
 * https://automaticaddison.com/how-to-convert-a-quaternion-to-a-rotation-matrix/
 */
Matrix4 &rotate(Matrix4 &matrix, const Quaternion &q)
{
    float r = q.r, i = q.i, j = q.j, k = q.k;
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
    matrix.identity();
    rotate(matrix, q);
    return matrix;
}

/**
 * Quickly inverts a translation and rotation matrix.
 * Transposes the rotation component and negates the translation component.
 * @returns The inverse of the rotation-translation matrix.
 */
Matrix4 quick_inverse(const Matrix4 &m)
{
    Matrix4 matrix;
    matrix.at(0, 1) = m.at(1, 0);
    matrix.at(0, 2) = m.at(2, 0);
    matrix.at(1, 0) = m.at(0, 1);
    matrix.at(1, 2) = m.at(2, 1);
    matrix.at(2, 0) = m.at(0, 2);
    matrix.at(2, 1) = m.at(1, 2);
    matrix.at(3, 0) = -m.at(0, 0) * m.at(0, 3) - m.at(1, 0) * m.at(1, 3) - m.at(2, 0) * m.at(2, 3);
    matrix.at(3, 1) = -m.at(0, 1) * m.at(0, 3) - m.at(1, 1) * m.at(1, 3) - m.at(2, 1) * m.at(2, 3);
    matrix.at(3, 2) = -m.at(0, 2) * m.at(0, 3) - m.at(1, 2) * m.at(1, 3) - m.at(2, 2) * m.at(2, 3);
    matrix.at(3, 3) = 1;
    return matrix;
}

/**
 * Creates a symmetric frustum with horizontal FOV.
 * https://www.songho.ca/opengl/gl_projectionmatrix.html
 */
Matrix4 projection(float fov, float aspect_ratio, float front, float back)
{
    const float DEG2RAD = std::acos(-1.0f) / 180;

    float tangent = std::tan(fov / 2 * DEG2RAD);
    float right = front * tangent;
    float top = right / aspect_ratio;

    Matrix4 matrix;
    matrix.at(0, 0) = front / right;
    matrix.at(1, 1) = front / top;
    matrix.at(2, 2) = -(back + front) / (back - front);
    matrix.at(2, 3) = -(2 * back * front) / (back - front);
    matrix.at(3, 2) = -1;
    return matrix;
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