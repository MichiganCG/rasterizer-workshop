#include "matrix.hpp"

Matrix4::Matrix4(std::vector<float> data) : data(data) {}
const Matrix4 Matrix4::IDENTITY = Matrix4({1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1});

Matrix4::Matrix4() : data(std::vector<float>(D_SIZE, 0.0f)) {}

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

std::ostream &operator<<(std::ostream &os, const Matrix4 &m)
{
    for (size_t i = 0; i < Matrix4::SIZE; ++i)
    {
        for (size_t j = 0; j < Matrix4::SIZE; ++j)
        {
            os << m[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}

Matrix4 operator*(const Matrix4 &m1, const Matrix4 &m2)
{
    Matrix4 temp;
    for (size_t i = 0; i < Matrix4::SIZE; ++i)
    {
        for (size_t j = 0; j < Matrix4::SIZE; ++j)
        {
            for (size_t k = 0; k < Matrix4::SIZE; ++k)
            {
                temp[i][j] += (m1[i][k] * m2[k][j]);
            }
        }
    }
    return temp;
}