#pragma once

namespace math
{
    struct Vector3_old
    {
        float x;
        float y;
        float z;

        Vector3_old() : x(0.0f), y(0.0f), z(0.0f) {}
        Vector3_old(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        void Normalize();

        float Magnitude() const;
        float MagnitudeSquared() const;

        float Dot(const Vector3_old& rhs) const;

        Vector3_old Cross(const Vector3_old& rhs) const;

        void Clear();

        Vector3_old operator+(const Vector3_old& rhs) const;
        void operator+=(const Vector3_old& rhs);

        Vector3_old operator-(const Vector3_old& rhs) const;
        void operator-=(const Vector3_old& rhs);

        Vector3_old operator*(const float value) const;
        void operator*=(const float value);

        float& operator[](unsigned int idx);
        float operator[](unsigned int idx) const;
        Vector3_old operator-() const { return { -x,-y,-z }; }
        bool operator==(const Vector3_old& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
        bool operator!=(const Vector3_old& rhs) const { return !(*this == rhs); }
    };
} 