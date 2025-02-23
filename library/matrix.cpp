#include "matrix.hpp"

const Matrix4 Matrix4::IDENTITY = Matrix4({1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1});

Matrix4::Matrix4(std::vector<float> data) : data(data) {}

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

Matrix4 &Matrix4::operator*=(const Matrix4 &rhs)
{
    Matrix4 temp;
    for (size_t i = 0; i < Matrix4::SIZE; ++i)
    {
        for (size_t j = 0; j < Matrix4::SIZE; ++j)
        {
            for (size_t k = 0; k < Matrix4::SIZE; ++k)
            {
                temp.at(i, j) += (at(i, k) * rhs.at(k, j));
            }
        }
    }
    return (*this = temp);
}

Matrix4 &Matrix4::operator*=(const float rhs)
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
