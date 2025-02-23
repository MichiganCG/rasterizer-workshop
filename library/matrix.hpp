#pragma once

#include <cmath>
#include <vector>
#include <ostream>

class Matrix4
{
private:
    std::vector<float> data;
    Matrix4(std::vector<float>);

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
    static const Matrix4 IDENTITY;

    Matrix4();
    Matrix4(const Matrix4 &);
    Matrix4 &operator=(const Matrix4 &);

    float &at(size_t row, size_t col) { return data[row * SIZE + col]; }
    float at(size_t row, size_t col) const { return data[row * SIZE + col]; }

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

    Matrix4 &operator*=(const Matrix4 &);
    Matrix4 &operator*=(const float);

    friend Matrix4 operator*(Matrix4 lhs, const Matrix4 &rhs) { return (lhs *= rhs); }
    friend Matrix4 operator*(Matrix4 lhs, const float rhs) { return (lhs *= rhs); }

    friend std::ostream &operator<<(std::ostream &, const Matrix4 &);
};
