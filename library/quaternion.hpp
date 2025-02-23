#pragma once

#include "vectors.hpp"
#include "matrix.hpp"

class Quaternion
{
public:
    float w, x, y, z;

    Quaternion();
    Quaternion(float, float, float, float);
    Quaternion(const Quaternion &);
    Quaternion &operator=(const Quaternion &);

    Vec3 forward();
    Vec3 up();
    Vec3 right();
    Vec3 bivector();

    void get_axis_angle(Vec3 &, float &);

    /**
     * Adds the rotation to this quaternion.
     */
    void rotate(const Vec3 &, const float);

    Quaternion &operator*=(const Quaternion &);
    Quaternion &operator/=(const float);

	friend std::ostream &operator<<(std::ostream &, const Quaternion &);
};

inline Quaternion operator*(Quaternion lhs, const Quaternion &rhs) { return (lhs *= rhs); }
inline Quaternion operator/(Quaternion lhs, const float rhs) { return (lhs /= rhs); }

inline float dot(const Quaternion &, const Quaternion &);
inline float magnitude_squared(const Quaternion &);
inline float magnitude(const Quaternion &);

Quaternion normalize(const Quaternion &);
Quaternion conjugate(const Quaternion &);

Quaternion local_rotation(const Vec3 &, const float);