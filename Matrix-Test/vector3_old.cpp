#include "vector3_old.h"
#include <iostream>
#include <cmath>
#include <cfloat>

using namespace math;

void Vector3_old::Normalize()
{
    float magnitudeInverse = 1.0f / Magnitude();
    x *= magnitudeInverse;
    y *= magnitudeInverse;
    z *= magnitudeInverse;
}

float Vector3_old::Magnitude() const
{
    float sum = x*x + y*y + z*z;
    return sqrtf(sum);
}

float Vector3_old::MagnitudeSquared() const
{
    return x*x + y*y + z*z;
}

float Vector3_old::Dot(const Vector3_old& rhs) const
{
    return x*rhs.x + y*rhs.y + z*rhs.z;
}

Vector3_old Vector3_old::Cross(const Vector3_old& rhs) const
{
    return Vector3_old(
        y*rhs.z - z*rhs.y,
        -x*rhs.z + z*rhs.x,
        x*rhs.y - y*rhs.x
    );
}

void Vector3_old::Clear()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
}

Vector3_old Vector3_old::operator+(const Vector3_old& rhs) const
{
    return Vector3_old(x + rhs.x, y + rhs.y, z + rhs.z);
}

void Vector3_old::operator+=(const Vector3_old& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
}

Vector3_old Vector3_old::operator-(const Vector3_old& rhs) const
{
    return Vector3_old(x - rhs.x, y - rhs.y, z - rhs.z);
}

void Vector3_old::operator-=(const Vector3_old& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
}

Vector3_old Vector3_old::operator*(const float value) const
{
    return Vector3_old(x * value, y * value, z * value);
}

void Vector3_old::operator*=(const float value)
{
    x *= value;
    y *= value;
    z *= value;
}

float Vector3_old::operator[](unsigned int idx) const
{
    switch (idx)
    {
    case 0:
        return x;

    case 1:
        return y;

    case 2:
        return z;
    
    default:
        std::cout << "Vector3::operator[]::Out of index" << std::endl;
        return FLT_MAX;
    }
}

float& Vector3_old::operator[](unsigned int idx){
    switch (idx)
    {
    case 0:
        return x;

    case 1:
        return y;

    case 2:
        return z;

    default:
        throw std::runtime_error("Vector3::operator[]::Out of index");
    }
}
