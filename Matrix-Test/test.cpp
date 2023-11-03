#include "pch.h"
#include "matrix3.h"
#include "matrix4.h"

#include "matrix3_old.h"
#include "matrix4_old.h"

const float EPSILON = 1e-5f;
const float LOOSE_EPSILON = 1e-3f;

using namespace math;

TEST(Matrix3Test, Constructor_DiagonalSingleValue) {
	Matrix3 m(7.0f);
	EXPECT_FLOAT_EQ(m.entries[0][0], 7.0f);
	EXPECT_FLOAT_EQ(m.entries[1][1], 7.0f);
	EXPECT_FLOAT_EQ(m.entries[2][2], 7.0f);

	EXPECT_FLOAT_EQ(m.entries[0][1], 0.0f);
	EXPECT_FLOAT_EQ(m.entries[0][2], 0.0f);
	EXPECT_FLOAT_EQ(m.entries[1][0], 0.0f);
	EXPECT_FLOAT_EQ(m.entries[1][2], 0.0f);
	EXPECT_FLOAT_EQ(m.entries[2][0], 0.0f);
	EXPECT_FLOAT_EQ(m.entries[2][1], 0.0f);
}

TEST(Matrix3Test, Constructor_DiagonalThreeValues) {
	Matrix3 m(1.0f, 2.0f, 3.0f);
	EXPECT_FLOAT_EQ(m.entries[0][0], 1.0f);
	EXPECT_FLOAT_EQ(m.entries[1][1], 2.0f);
	EXPECT_FLOAT_EQ(m.entries[2][2], 3.0f);

	EXPECT_FLOAT_EQ(m.entries[0][1], 0.0f);
	EXPECT_FLOAT_EQ(m.entries[0][2], 0.0f);
	EXPECT_FLOAT_EQ(m.entries[1][0], 0.0f);
	EXPECT_FLOAT_EQ(m.entries[1][2], 0.0f);
	EXPECT_FLOAT_EQ(m.entries[2][0], 0.0f);
	EXPECT_FLOAT_EQ(m.entries[2][1], 0.0f);
}

TEST(Matrix3Test, Transpose) {
	Matrix3 m(1.0f, 0.0f, 3.0f,
		0.0f, 2.0f, 0.0f,
		3.0f, 0.0f, 3.0f);
	Matrix3 transposed = m.Transpose();

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			EXPECT_FLOAT_EQ(transposed.entries[j][i], m.entries[i][j]);
		}
	}
}
TEST(Matrix3Test, NonInvertibleMatrix) {
	Matrix3 m1(1, 2, 3, 4, 5, 6, 7, 8, 9);
	EXPECT_THROW(m1.Inverse(), std::runtime_error);
}

TEST(Matrix3Test, MatrixInverse) {
	// Define a sample column-major matrix.
	Matrix3 A;
	A.entries[0][0] = -1.0f; A.entries[1][0] = 3.0f; A.entries[2][0] = 5.0f;
	A.entries[0][1] = 2.0f; A.entries[1][1] = 4.0f; A.entries[2][1] = -6.0f;
	A.entries[0][2] = 3.0f; A.entries[1][2] = -5.0f; A.entries[2][2] = 7.0f;

	// Compute the Inverse using your function.
	Matrix3 A_inv = A.Inverse();

	// Multiply the matrix and its Inverse.
	Matrix3 product = A * A_inv;

	// Define an identity matrix.
	Matrix3 I;

	// Check if the product of A and its Inverse is close to the identity matrix.
	for (int col = 0; col < 3; ++col) {
		for (int row = 0; row < 3; ++row) {
			//EXPECT_FLOAT_EQ(product.entries[col][row]-I.entries[col][row]);
			if (col == row) {
				EXPECT_NEAR(product.entries[col][row], I.entries[col][row], EPSILON);
			}
		}
	}
}

TEST(Matrix4Test, Constructor_DiagonalSingleValue) {
	Matrix4 m(9.0f);
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (i == j) {
				EXPECT_FLOAT_EQ(m[i * 4 + j], 9.0f);
			}
			else {
				EXPECT_FLOAT_EQ(m[i * 4 + j], 0.0f);
			}
		}
	}
}

TEST(Matrix4Test, Constructor_DiagonalFourValues) {
	Matrix4 m(1.0f, 2.0f, 3.0f, 4.0f);
	EXPECT_FLOAT_EQ(m[0], 1.0f);
	EXPECT_FLOAT_EQ(m[5], 2.0f);
	EXPECT_FLOAT_EQ(m[10], 3.0f);
	EXPECT_FLOAT_EQ(m[15], 4.0f);

	EXPECT_FLOAT_EQ(m[1], 0.0f);
	EXPECT_FLOAT_EQ(m[2], 0.0f);
	EXPECT_FLOAT_EQ(m[3], 0.0f);
	EXPECT_FLOAT_EQ(m[4], 0.0f);
	EXPECT_FLOAT_EQ(m[6], 0.0f);
	EXPECT_FLOAT_EQ(m[7], 0.0f);
	EXPECT_FLOAT_EQ(m[8], 0.0f);
	EXPECT_FLOAT_EQ(m[9], 0.0f);
	EXPECT_FLOAT_EQ(m[11], 0.0f);
	EXPECT_FLOAT_EQ(m[12], 0.0f);
	EXPECT_FLOAT_EQ(m[13], 0.0f);
	EXPECT_FLOAT_EQ(m[14], 0.0f);
}

TEST(Matrix4Test, Transpose) {
	Matrix4 m(1.0f, 0.0f, 3.0f, 0.0f,
		0.0f, 2.0f, 0.0f, 0.0f,
		3.0f, 0.0f, 3.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 4.0f);

	Matrix4 transposed = m.Transpose();

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			EXPECT_FLOAT_EQ(transposed[i * 4 + j], m[j * 4 + i]);
		}
	}
}
TEST(Matrix4Test, Matrix4_Inverse_Multiplication_intergers) {
	Matrix4 M{
		-1,    3,     5,    7,
		9,    -10,  12,  14,
		-16,  18,  -19,  -21,
		23,  -25,  27,   29
	};

	Matrix4 M_inv = M.Inverse();


	Matrix4 product = M * M_inv;

	Matrix4 I; //default, identity

	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			EXPECT_NEAR(product[col * 4 + row], I[col * 4 + row], LOOSE_EPSILON);
		}
	}
}
TEST(Matrix4Test, Matrix4_Inverse_Multiplication_floats) {
	Matrix4 M{
		-1.2,    3.4,     5.6,    7.8,
		9.0,    -10.1,  12.3,  14.15,
		-16.7,  18.0,  -19,  -20.2,
		23.4,  -25.6,  27.8,   29
	};
	Matrix4 M_inv = M.Inverse();

	Matrix4 product = M * M_inv;

	Matrix4 I; //default, identity

	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			if (col == row) {
				EXPECT_NEAR(product[col * 4 + row], I[col * 4 + row], LOOSE_EPSILON);
			}

		}
	}
}
TEST(Matrix3_Old_vs_New, Matrix3_Multiplication) {
	Matrix3 m ={ -1,2,3,4,5,6,-7,8,9};
	Matrix3_old mOld = { -1,2,3,4,5,6,-7,8,9 };

	Vector3 v{ 6,-23,12 };
	Vector3_old vOld{ 6,-23,12 };

	Vector3 r1 = m * v;
	Vector3_old r2 = mOld.Transpose() * vOld;


	for (int i{}; i < 3; ++i) {
		EXPECT_NEAR(r1[i], r2[i], EPSILON);
	}
}
TEST(Matrix4_Old_vs_New, Matrix4_Multiplication) {
	Matrix4 m1 =		{ -1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16 };
	Matrix4_old m1Old = { -1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16 };

	Matrix4 m2 =		{ 6,2,8,-2,6,45,1,5,-9,3,1,4,-9,20,65,-21 };
	Matrix4_old m2Old = { 6,2,8,-2,6,45,1,5,-9,3,1,4,-9,20,65,-21 };

	Matrix4 r1 =m1 * m2;
	Matrix4_old r2 = m1Old * m2Old;


	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			//note that idx are different.
			EXPECT_NEAR(r1[col * 4 + row], r2[row * 4 + col], LOOSE_EPSILON);
		}
	}
}
TEST(Matrix4_Old_vs_New, Matrix4_MultiplicationAssignment) {
	Matrix4 m1 = { -1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16 };
	Matrix4 m2 = { 6,2,8,-2,6,45,1,5,-9,3,1,4,-9,20,65,-21 };

	Matrix4_old m1Old = { -1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16 };
	Matrix4_old m2Old = { 6,2,8,-2,6,45,1,5,-9,3,1,4,-9,20,65,-21 };

	m1 *= m2;
	m1Old *= m2Old;


	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			//note that idx are different.
			EXPECT_NEAR(m1[col * 4 + row], m1Old[row * 4 + col], EPSILON);
		}
	}
}
TEST(Matrix4_Old_vs_New, UpdateTransformMatrix) {
	std::array<int, 3> position{ 9,-12,3 };
	std::array<int, 4> orientation{ 5,-6,7,-8 };
	
	Matrix4 localToWorldCoord;	
	Matrix4_old localToWorldCoordOld;
	//---------------
	{
		// First column
		localToWorldCoord.columns[0].m128_f32[0] = 1.0f - 2.0f * (orientation[1] * orientation[1] + orientation[2] * orientation[2]);
		localToWorldCoord.columns[0].m128_f32[1] = 2.0f * (orientation[0] * orientation[1] + orientation[3] * orientation[2]);
		localToWorldCoord.columns[0].m128_f32[2] = 2.0f * (orientation[0] * orientation[2] - orientation[3] * orientation[1]);
		localToWorldCoord.columns[0].m128_f32[3] = 0.0f; // Assuming homogeneous coordinate for direction vectors is 0

		// Second column
		localToWorldCoord.columns[1].m128_f32[0] = 2.0f * (orientation[0] * orientation[1] - orientation[3] * orientation[2]);
		localToWorldCoord.columns[1].m128_f32[1] = 1.0f - 2.0f * (orientation[0] * orientation[0] + orientation[2] * orientation[2]);
		localToWorldCoord.columns[1].m128_f32[2] = 2.0f * (orientation[1] * orientation[2] + orientation[3] * orientation[0]);
		localToWorldCoord.columns[1].m128_f32[3] = 0.0f; // Assuming homogeneous coordinate for direction vectors is 0

		// Third column
		localToWorldCoord.columns[2].m128_f32[0] = 2.0f * (orientation[0] * orientation[2] + orientation[3] * orientation[1]);
		localToWorldCoord.columns[2].m128_f32[1] = 2.0f * (orientation[1] * orientation[2] - orientation[3] * orientation[0]);
		localToWorldCoord.columns[2].m128_f32[2] = 1.0f - 2.0f * (orientation[0] * orientation[0] + orientation[1] * orientation[1]);
		localToWorldCoord.columns[2].m128_f32[3] = 0.0f; // Assuming homogeneous coordinate for direction vectors is 0

		// Fourth column (position)
		localToWorldCoord.columns[3].m128_f32[0] = position[0];
		localToWorldCoord.columns[3].m128_f32[1] = position[1];
		localToWorldCoord.columns[3].m128_f32[2] = position[2];
		localToWorldCoord.columns[3].m128_f32[3] = 1.0f; // Homogeneous coordinate for position is 1
	}
	{
		localToWorldCoordOld[0] =
			1.0f - 2.0f * (orientation[1] * orientation[1] + orientation[2] * orientation[2]);
		localToWorldCoordOld[1] =
			2.0f * (orientation[0] * orientation[1] - orientation[3] * orientation[2]);
		localToWorldCoordOld[2] =
			2.0f * (orientation[0] * orientation[2] + orientation[3] * orientation[1]);
		localToWorldCoordOld[3] = position[0];

		localToWorldCoordOld[4] =
			2.0f * (orientation[0] * orientation[1] + orientation[3] * orientation[2]);
		localToWorldCoordOld[5] =
			1.0f - 2.0f * (orientation[0] * orientation[0] + orientation[2] * orientation[2]);
		localToWorldCoordOld[6] =
			2.0f * (orientation[1] * orientation[2] - orientation[3] * orientation[0]);
		localToWorldCoordOld[7] = position[1];

		localToWorldCoordOld[8] =
			2.0f * (orientation[0] * orientation[2] - orientation[3] * orientation[1]);
		localToWorldCoordOld[9] =
			2.0f * (orientation[1] * orientation[2] + orientation[3] * orientation[0]);
		localToWorldCoordOld[10] =
			1.0f - 2.0f * (orientation[0] * orientation[0] + orientation[1] * orientation[1]);
		localToWorldCoordOld[11] = position[2];
	}

	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			EXPECT_NEAR(localToWorldCoordOld[col * 4 + row], localToWorldCoord[row * 4 + col], EPSILON);
		}
	}
}

TEST(Matrix4_Old_vs_New, TransformInertiaTensor) {
	Matrix4 localToWorldCoord = { -1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16 };
	Matrix3 inverseInertiaTensor;
	Matrix3 inverseInertiaTensorWorld;

	Matrix4_old localToWorldCoordOld = { -1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16 };
	Matrix3_old inverseInertiaTensorOld;
	Matrix3_old inverseInertiaTensorWorldOld;
	{
		Matrix3 inertiaTensor;
		float mass = 5;
		float k = mass / 12;
		float x =  2.0f;
		float y =  3.0f;
		float z =  4.0f;
		inertiaTensor[0] = k * (y * y + z * z);
		inertiaTensor[4] = k * (x * x + z * z);
		inertiaTensor[8] = k * (y * y + x * x);
		inverseInertiaTensor = inertiaTensor.Inverse();
	}
	{
		Matrix3_old inertiaTensorOld;
		float mass = 5;
		float k = mass / 12;
		float x = 2.0f;
		float y = 3.0f;
		float z = 4.0f;
		inertiaTensorOld[0] = k * (y * y + z * z);
		inertiaTensorOld[4] = k * (x * x + z * z);
		inertiaTensorOld[8] = k * (y * y + x * x);
		inverseInertiaTensorOld = inertiaTensorOld.Inverse();
	}
	//---------------
	{
		Matrix3 rotationMatrix = localToWorldCoord.Extract3x3Matrix();
		inverseInertiaTensorWorld = (rotationMatrix * inverseInertiaTensor) * rotationMatrix.Transpose();
	}
	{
		Matrix3_old rotationMatrixOld = localToWorldCoordOld.Extract3x3Matrix();
		inverseInertiaTensorWorldOld = (rotationMatrixOld * inverseInertiaTensorOld) * rotationMatrixOld.Transpose();
	}

	for (int col = 0; col < 3; ++col) {
		for (int row = 0; row < 3; ++row) {
			EXPECT_NEAR(inverseInertiaTensorWorld[col * 3 + row], inverseInertiaTensorWorldOld[row * 3 + col], EPSILON);
		}
	}
}

TEST(Matrix4_Old_vs_New, SetAngularVelocity) {
	Matrix4 localToWorldCoord = {-1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16};
	Vector3 angularVelocity;
	Vector3 vec{ 51.3,-9.23,11.5 };

	Matrix4_old localToWorldCoordOld = { -1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16 };
	Vector3_old angularVelocityOld;
	Vector3_old vecOld{ 51.3,-9.23,11.5 };

	//---------------
	{
		Matrix3 rotationMatrix = localToWorldCoord.Extract3x3Matrix();
		angularVelocity = rotationMatrix * vec; //NO TRANSPOSE anymore!, (multiplies columns of the matrix not rows)
	}
	{
		Matrix3_old rotationMatrixOld = localToWorldCoordOld.Extract3x3Matrix();
		angularVelocityOld = rotationMatrixOld.Transpose() * vecOld;
	}
	//---------------

	for (int i{}; i < 3; ++i) {
		EXPECT_NEAR(angularVelocity[i], angularVelocityOld[i], EPSILON);
	}
}

TEST(Matrix4_Old_vs_New, GetAxis) {
	Matrix4 localToWorldCoord = { -1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16 };
	Matrix4_old localToWorldCoordOld = { -1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16 };

	Vector3 a1(
		localToWorldCoord[0],
		localToWorldCoord[1],
		localToWorldCoord[2]
	);
	Vector3 a2(
		localToWorldCoord[4],
		localToWorldCoord[5],
		localToWorldCoord[6]
	);
	Vector3 a3(
		localToWorldCoord[8],
		localToWorldCoord[9],
		localToWorldCoord[10]
	);
	Vector3 a1Old(
		localToWorldCoordOld[0],
		localToWorldCoordOld[4],
		localToWorldCoordOld[8]
	);
	Vector3 a2Old(
		localToWorldCoordOld[1],
		localToWorldCoordOld[5],
		localToWorldCoordOld[9]
	);
	Vector3 a3Old(
		localToWorldCoordOld[2],
		localToWorldCoordOld[6],
		localToWorldCoordOld[10]
	);

	for (int i{}; i < 3; ++i) {
		EXPECT_NEAR(a1[i], a1Old[i], EPSILON);
	}
	for (int i{}; i < 3; ++i) {
		EXPECT_NEAR(a2[i], a2Old[i], EPSILON);
	}
	for (int i{}; i < 3; ++i) {
		EXPECT_NEAR(a3[i], a3Old[i], EPSILON);
	}
}

TEST(Matrix4_Old_vs_New, ConvertToGLM) {
	Matrix4 localToWorldCoord = { -1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16 };
	Matrix4_old localToWorldCoordOld = { -1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16 };

	glm::mat4 m=localToWorldCoord.ConvertToGLM();
	glm::mat4 mOld=localToWorldCoordOld.ConvertToGLM();

	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			EXPECT_NEAR(m[col][row], mOld[col][row], EPSILON);
		}
	}
}
TEST(Matrix4_Old_vs_New, Matrix4_X_Vector3) {
	Matrix4 localToWorldCoord = { -1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16 };
	Vector3 angularVelocity;
	Vector3 vec{ 51.3,-9.23,11.5 };

	Matrix4_old localToWorldCoordOld = { -1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16 };
	Vector3_old angularVelocityOld;
	Vector3_old vecOld{ 51.3,-9.23,11.5 };

	//---------------
	{
		angularVelocity = localToWorldCoord * vec;
	}
	{
		angularVelocityOld = localToWorldCoordOld * vecOld;
	}
	//---------------

	for (int i{}; i < 3; ++i) {
		EXPECT_NEAR(angularVelocity[i], angularVelocityOld[i], EPSILON);
	}
}

TEST(Matrix4_Old_vs_New, Extract3x3_Multiplication) {
	Matrix4 localToWorldCoord = { -1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16 };
	Vector3 angularVelocity;
	Vector3 vec{ 51.3,-9.23,11.5 };

	Matrix4_old localToWorldCoordOld = { -1,2,3,4,5,6,-7,8,9,10,11,12,13,-14,15,16 };
	Vector3_old angularVelocityOld;
	Vector3_old vecOld{ 51.3,-9.23,11.5 };

	//---------------
	{
		angularVelocity = localToWorldCoord.Extract3x3Matrix() * vec;
	}
	{
		Matrix3_old rotationMatrixOld = localToWorldCoordOld.Extract3x3Matrix();
		angularVelocityOld = rotationMatrixOld.Transpose() * vecOld;
	}
	//---------------

	for (int i{}; i < 3; ++i) {
		EXPECT_NEAR(angularVelocity[i], angularVelocityOld[i], EPSILON);
	}
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}