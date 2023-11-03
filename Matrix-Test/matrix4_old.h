#pragma once

#include "vector3_old.h"
#include <array>
#include <glm/glm.hpp>

namespace math
{
    struct Matrix4_old
    {
        std::array<std::array<float, 4>, 4> entries;

        Matrix4_old(float value=1.f);
        Matrix4_old(float v1, float v2, float v3, float v4);
        Matrix4_old(float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8, float v9, float v10, float v11, float v12, float v13, float v14, float v15, float v16);

        Matrix4_old Transpose() const;
        Matrix4_old Inverse() const;

        Matrix4_old operator+(const Matrix4_old& other) const;
        Matrix4_old& operator+=(const Matrix4_old& other);
        Matrix4_old operator-(const Matrix4_old& other) const;
        Matrix4_old& operator-=(const Matrix4_old& other);
        Matrix4_old operator*(const Matrix4_old& other) const;
        Matrix4_old& operator*=(const Matrix4_old& other);
        Vector3_old operator*(const Vector3_old& vec) const;
        Matrix4_old operator*(const float value) const;
        Matrix4_old& operator*=(const float value);
        float operator[](int idx) const;
        float& operator[](int idx);
        Matrix4_old& operator=(const Matrix4_old& other);
        
        bool operator==(const Matrix4_old& other);
        bool operator!=(const Matrix4_old& other);

        Matrix3_old Extract3x3Matrix() const;

        Vector3_old ExtractTranslation() const;//temp, not being used for now
        Vector3_old ExtractScale() const;

        glm::mat4 ConvertToGLM()const noexcept;
    };    
}