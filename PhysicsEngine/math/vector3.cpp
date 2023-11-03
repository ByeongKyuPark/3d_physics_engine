#include "vector3.h"
#include <iostream>
#include <cmath>
#include <cfloat>

using namespace math;

void Vector3::Normalize()
{
    float magnitudeInverse = 1.0f / Length();
    x *= magnitudeInverse;
    y *= magnitudeInverse;
    z *= magnitudeInverse;
}

float Vector3::Length() const
{
    float sum = x*x + y*y + z*z;
    return sqrtf(sum);
}

float Vector3::LengthSquared() const
{
    return x*x + y*y + z*z;
}

float Vector3::Dot(const Vector3& rhs) const
{
    return x*rhs.x + y*rhs.y + z*rhs.z;
}

Vector3 Vector3::Cross(const Vector3& rhs) const
{
    return Vector3(
        y*rhs.z - z*rhs.y,
        -x*rhs.z + z*rhs.x,
        x*rhs.y - y*rhs.x
    );
}

void Vector3::Clear()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
}

Vector3 Vector3::operator+(const Vector3& rhs) const
{
    return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

void Vector3::operator+=(const Vector3& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
}

Vector3 Vector3::operator-(const Vector3& rhs) const
{
    return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

void Vector3::operator-=(const Vector3& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
}

Vector3 Vector3::operator*(const float value) const
{
    return Vector3(x * value, y * value, z * value);
}

void Vector3::operator*=(const float value)
{
    x *= value;
    y *= value;
    z *= value;
}

float Vector3::operator[](unsigned int idx) const
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

float& Vector3::operator[](unsigned int idx){
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
