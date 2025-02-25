#pragma once

#include "vectors.hpp"

#include <ostream>

class Quaternion
{
public:
    float r, i, j, k;

    Quaternion();
    Quaternion(float, float, float, float);
    Quaternion(const Quaternion &);
    Quaternion &operator=(const Quaternion &);

    Vec3 forward();
    Vec3 up();
    Vec3 left();
    Vec3 bivector();

    void get_axis_angle(Vec3 &, float &);

    void rotate(const Vec3 &, float);

    Quaternion &operator*=(const Quaternion &);
    Quaternion &operator*=(float);
    Quaternion &operator/=(float);

	friend std::ostream &operator<<(std::ostream &, const Quaternion &);
};

inline Quaternion operator*(Quaternion lhs, const Quaternion &rhs) { return (lhs *= rhs); }
inline Quaternion operator*(Quaternion lhs, float rhs) { return (lhs *= rhs); }
inline Quaternion operator/(Quaternion lhs, float rhs) { return (lhs /= rhs); }

inline float dot(const Quaternion &, const Quaternion &);
inline float magnitude_squared(const Quaternion &);
inline float magnitude(const Quaternion &);

Quaternion normalize(const Quaternion &);
Quaternion conjugate(const Quaternion &);

Quaternion local_rotation(const Vec3 &, float);