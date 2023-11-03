#include "matrix3_old.h"
#include "matrix4_old.h"

#include <iostream>

using namespace math;

Matrix4_old::Matrix4_old(float value){
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            entries[j][i] = (i == j) ? value : 0.0f;
        }
    }
}

Matrix4_old::Matrix4_old(float v1, float v2, float v3, float v4) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            entries[j][i] = 0.0f;
        }
    }

    entries[0][0] = v1;
    entries[1][1] = v2;
    entries[2][2] = v3;
    entries[3][3] = v4;
}

Matrix4_old::Matrix4_old(float v1, float v2, float v3, float v4,
    float v5, float v6, float v7, float v8,
    float v9, float v10, float v11, float v12,
    float v13, float v14, float v15, float v16)
{
    // Column 1
    entries[0][0] = v1;
    entries[1][0] = v2;
    entries[2][0] = v3;
    entries[3][0] = v4;

    // Column 2
    entries[0][1] = v5;
    entries[1][1] = v6;
    entries[2][1] = v7;
    entries[3][1] = v8;

    // Column 3
    entries[0][2] = v9;
    entries[1][2] = v10;
    entries[2][2] = v11;
    entries[3][2] = v12;

    // Column 4
    entries[0][3] = v13;
    entries[1][3] = v14;
    entries[2][3] = v15;
    entries[3][3] = v16;
}

Matrix4_old Matrix4_old::Transpose() const {
    Matrix4_old result;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.entries[i][j] = entries[j][i];
        }
    }

    return result;
}
Matrix4_old Matrix4_old::Inverse() const
{
    float inv[16];

    // Local variables for often-used matrix elements
    float m0 = operator[](0), m1 = operator[](1), m2 = operator[](2), m3 = operator[](3),
        m4 = operator[](4), m5 = operator[](5), m6 = operator[](6), m7 = operator[](7),
        m8 = operator[](8), m9 = operator[](9), m10 = operator[](10), m11 = operator[](11),
        m12 = operator[](12), m13 = operator[](13), m14 = operator[](14), m15 = operator[](15);

    // Compute the Inverse using adjugate and determinant
    inv[0] = m5 * (m10 * m15 - m11 * m14) + m9 * (m7 * m14 - m6 * m15) + m13 * (m6 * m11 - m7 * m10);
    inv[1] = m1 * (m11 * m14 - m10 * m15) + m9 * (m2 * m15 - m3 * m14) + m13 * (m3 * m10 - m2 * m11);
    inv[2] = m1 * (m6 * m15 - m7 * m14) + m5 * (m3 * m14 - m2 * m15) + m13 * (m2 * m7 - m3 * m6);
    inv[3] = m1 * (m7 * m10 - m6 * m11) + m5 * (m2 * m11 - m3 * m10) + m9 * (m3 * m6 - m2 * m7);

    inv[4] = m4 * (m11 * m14 - m10 * m15) + m8 * (m6 * m15 - m7 * m14) + m12 * (m7 * m10 - m6 * m11);
    inv[5] = m0 * (m10 * m15 - m11 * m14) + m8 * (m3 * m14 - m2 * m15) + m12 * (m2 * m11 - m3 * m10);
    inv[6] = m0 * (m7 * m14 - m6 * m15) + m4 * (m2 * m15 - m3 * m14) + m12 * (m3 * m6 - m2 * m7);
    inv[7] = m0 * (m6 * m11 - m7 * m10) + m4 * (m3 * m10 - m2 * m11) + m8 * (m2 * m7 - m3 * m6);

    inv[8] = m4 * (m9 * m15 - m11 * m13) + m8 * (m7 * m13 - m5 * m15) + m12 * (m5 * m11 - m7 * m9);
    inv[9] = m0 * (m11 * m13 - m9 * m15) + m8 * (m1 * m15 - m3 * m13) + m12 * (m3 * m9 - m1 * m11);
    inv[10] = m0 * (m5 * m15 - m7 * m13) + m4 * (m3 * m13 - m1 * m15) + m12 * (m1 * m7 - m3 * m5);
    inv[11] = m0 * (m7 * m9 - m5 * m11) + m4 * (m1 * m11 - m3 * m9) + m8 * (m3 * m5 - m1 * m7);

    inv[12] = m4 * (m10 * m13 - m9 * m14) + m8 * (m5 * m14 - m6 * m13) + m12 * (m6 * m9 - m5 * m10);
    inv[13] = m0 * (m9 * m14 - m10 * m13) + m8 * (m2 * m13 - m1 * m14) + m12 * (m1 * m10 - m2 * m9);
    inv[14] = m0 * (m6 * m13 - m5 * m14) + m4 * (m1 * m14 - m2 * m13) + m12 * (m2 * m5 - m1 * m6);
    inv[15] = m0 * (m5 * m10 - m6 * m9) + m4 * (m2 * m9 - m1 * m10) + m8 * (m1 * m6 - m2 * m5);

    // Compute the determinant using the adjugate formula
    float determinant = m0 * inv[0] + m1 * inv[4] + m2 * inv[8] + m3 * inv[12];

    if (fabs(determinant) < 1e-6f)
    {
        std::cerr << "MATRIX4::singular, inverse does not exist." << std::endl;
        return *this;
    }

    float invDet = 1.0f / determinant;
    Matrix4_old result;

    for (int i = 0; i < 16; i++) {
        result[i] = inv[i] * invDet;
    }

    return result;
}

Matrix4_old Matrix4_old::operator+(const Matrix4_old& other) const{
    Matrix4_old result;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.entries[j][i] = entries[j][i] + other.entries[j][i];
        }
    }

    return result;
}

Matrix4_old& Matrix4_old::operator+=(const Matrix4_old& other)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            entries[j][i] +=  other.entries[j][i];
        }
    }
    return *this;
}

Matrix4_old Matrix4_old::operator-(const Matrix4_old& other) const
{
    Matrix4_old result;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.entries[j][i] = entries[j][i] - other.entries[j][i];
        }
    }

    return result;
}

Matrix4_old& Matrix4_old::operator-=(const Matrix4_old& other)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            entries[j][i] -= other.entries[j][i];
        }
    }
    return *this;
}

Matrix4_old Matrix4_old::operator*(const Matrix4_old& other) const {
    Matrix4_old result;

    // Zero out
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.entries[j][i] = 0.0f;
        }
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                result.entries[j][i] += entries[k][i] * other.entries[j][k];
            }
        }
    }

    return result;
}


Matrix4_old& Matrix4_old::operator*=(const Matrix4_old& other) {
    Matrix4_old result;

    // Zero out
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.entries[j][i] = 0.0f;
        }
    }

    // Multiplication logic
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                result.entries[j][i] += entries[k][i] * other.entries[j][k];
            }
        }
    }

    // Assign the result back to the original matrix
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            entries[j][i] = result.entries[j][i];
        }
    }

    return *this;
}


Vector3_old Matrix4_old::operator*(const Vector3_old& vec) const {
    // Treat the vector as a Vector4 with a w-component of 1
    float x = entries[0][0] * vec.x + entries[1][0] * vec.y + entries[2][0] * vec.z + entries[3][0];
    float y = entries[0][1] * vec.x + entries[1][1] * vec.y + entries[2][1] * vec.z + entries[3][1];
    float z = entries[0][2] * vec.x + entries[1][2] * vec.y + entries[2][2] * vec.z + entries[3][2];
    float w = entries[0][3] * vec.x + entries[1][3] * vec.y + entries[2][3] * vec.z + entries[3][3];

    // If the matrix represents projective transformations,
    if (w != 1.0f && w != 0.0f) {
        x /= w;
        y /= w;
        z /= w;
    }

    return Vector3_old(x, y, z);
}

Matrix4_old Matrix4_old::operator*(const float value) const
{
    Matrix4_old result;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.entries[j][i] = entries[j][i] * value;
        }
    }

    return result;
}

Matrix4_old& Matrix4_old::operator*=(const float value)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            entries[j][i] *= value;
        }
    }
    return *this;
}

float Matrix4_old::operator[](int idx) const {
    if (idx < 0 || idx > 15) {
        throw std::out_of_range("Index out of bounds for Matrix4");
    }
    return entries[idx % 4][idx / 4];
}

float& Matrix4_old::operator[](int idx) {
    if (idx < 0 || idx > 15) {
        throw std::out_of_range("Index out of bounds for Matrix4");
    }
    return entries[idx % 4][idx / 4];
}


Matrix4_old& Matrix4_old::operator=(const Matrix4_old& other)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            entries[j][i] = other.entries[j][i];
        }
    }
    return *this;
}

bool Matrix4_old::operator==(const Matrix4_old& other)
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (entries[j][i] != other.entries[j][i]) {
                return false;
            }
        }
    }
    return true;
}

bool Matrix4_old::operator!=(const Matrix4_old& other)
{
    return !(*this == other);
}

Matrix3_old Matrix4_old::Extract3x3Matrix() const {
    Matrix3_old result;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result.entries[j][i] = entries[j][i];
        }
    }

    return result;
}

Vector3_old Matrix4_old::ExtractTranslation() const {
    Vector3_old result;
    result.x = entries[0][3];
    result.y = entries[1][3];
    result.z = entries[2][3];
    return result;
}

Vector3_old Matrix4_old::ExtractScale() const {
    Vector3_old scale;

    scale.x = sqrt(entries[0][0] * entries[0][0] + entries[1][0] * entries[1][0] + entries[2][0] * entries[2][0]);
    scale.y = sqrt(entries[0][1] * entries[0][1] + entries[1][1] * entries[1][1] + entries[2][1] * entries[2][1]);
    scale.z = sqrt(entries[0][2] * entries[0][2] + entries[1][2] * entries[1][2] + entries[2][2] * entries[2][2]);

    return scale;
}

glm::mat4 Matrix4_old::ConvertToGLM() const noexcept{
    // The glm::mat4 constructor is in column-major order.
    return glm::mat4(
        operator[](0), operator[](4), operator[](8), operator[](12),
        operator[](1), operator[](5), operator[](9), operator[](13),
        operator[](2), operator[](6), operator[](10), operator[](14),
        operator[](3), operator[](7), operator[](11), operator[](15)
    );
}
