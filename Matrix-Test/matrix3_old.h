#pragma once

#include "vector3_old.h"
#include <array>

namespace math
{
    struct Matrix3_old
    {
        std::array<std::array<float, 3>, 3> entries;

        Matrix3_old(float diagonal=1.f); 
        Matrix3_old(float diagonal1, float diagonal2, float diagonal3);
        Matrix3_old(float e1, float e2, float e3, float e4, float e5, float e6, float e7, float e8, float e9);

        Matrix3_old Transpose() const;
        Matrix3_old Inverse() const;

        Matrix3_old operator+(const Matrix3_old& other) const;
        Matrix3_old& operator+=(const Matrix3_old& other);

        Matrix3_old operator-(const Matrix3_old& other) const;
        Matrix3_old& operator-=(const Matrix3_old& other);

        Matrix3_old operator*(const Matrix3_old& other) const;
        Matrix3_old& operator*=(const Matrix3_old& other);

        Vector3_old operator*(const Vector3_old& vec) const;

        Matrix3_old operator*(const float value) const;
        Matrix3_old& operator*=(const float value);

        Matrix3_old& operator=(const Matrix3_old& other);

        bool operator==(const Matrix3_old& other)const;
        bool operator!=(const Matrix3_old& other)const;

        float operator[](int idx) const;
        float& operator[](int idx);

        void SetDiagonal(float value);
    };
}