#pragma once

#include "vector3.h"
#include <array>
#include <emmintrin.h>  // For SSE2 instruction sets
#include <opengl/glm/glm.hpp>

namespace math
{
    struct Matrix4
    {
        alignas(16) std::array<__m128, 4> columns;


        Matrix4(float value=1.f);
        Matrix4(float v1, float v2, float v3, float v4);
        Matrix4(float e00, float e01, float e02, float e03, float e10, float e11, float e12, float e13, float e20, float e21, float e22, float e23, float e30, float e31, float e32, float e33);
        Matrix4 Transpose() const;
        Matrix4 Inverse() const;

        Matrix4 operator+(const Matrix4& other) const;
        Matrix4& operator+=(const Matrix4& other);
        Matrix4 operator-(const Matrix4& other) const;
        Matrix4& operator-=(const Matrix4& other);
        Matrix4 operator*(const Matrix4& other) const;
        Matrix4& operator*=(const Matrix4& other);
        Vector3 operator*(const Vector3& vec) const;
        Matrix4 operator*(const float value) const;
        Matrix4& operator*=(const float value);
        float operator[](int idx) const;
        float& operator[](int idx);
        Matrix4& operator=(const Matrix4& other);
        
        bool operator==(const Matrix4& other)const;
        bool operator!=(const Matrix4& other)const;

        Matrix3 Extract3x3Matrix() const;
        glm::mat4 ConvertToGLM()const noexcept;
    };    
}