#pragma once

#include <cmath>
#include <vector>

class Matrix4
{
private:
    std::vector<float> data;

    class Proxy
    {
        friend class Matrix4;
        size_t row;
        Matrix4 &m;
        Proxy(Matrix4 &m, size_t row) : row(row), m(m) {}

    public:
        float &operator[](size_t col) { return m.at(row, col); }
    };

    class ConstProxy
    {
        friend class Matrix4;
        size_t row;
        const Matrix4 &m;
        ConstProxy(const Matrix4 &m, size_t row) : row(row), m(m) {}

    public:
        float operator[](size_t col) const { return m.at(row, col); }
    };

public:
    Matrix4() : data(std::vector<float>(16, 0.0f)) {}

    float &at(size_t row, size_t col) { return data[row * 4 + col]; }
    float at(size_t row, size_t col) const { return data[row * 4 + col]; }

    Proxy operator[](size_t row) { return {*this, row}; }
    ConstProxy operator[](size_t row) const { return {*this, row}; }

    
};
