#pragma once

namespace math
{
    struct Vector3
    {
        float x;
        float y;
        float z;

        Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
        Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        void Normalize();

        float Length() const;
        float LengthSquared() const;

        float Dot(const Vector3& rhs) const;

        Vector3 Cross(const Vector3& rhs) const;

        void Clear();

        Vector3 operator+(const Vector3& rhs) const;
        void operator+=(const Vector3& rhs);

        Vector3 operator-(const Vector3& rhs) const;
        void operator-=(const Vector3& rhs);

        Vector3 operator*(const float value) const;
        void operator*=(const float value);

        float& operator[](unsigned int idx);
        float operator[](unsigned int idx) const;
        Vector3 operator-() const { return { -x,-y,-z }; }
        bool operator==(const Vector3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
        bool operator!=(const Vector3& rhs) const { return !(*this == rhs); }
    };
} 