#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cmath>
#include <cassert>
#include <Vector3.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Matrix4x4 class
////////////////////////////////////////////////////////////////////////////////////////////
class Matrix4x4 {
public:

	float m[4][4];

	static Matrix4x4 MakeIdentity();

	//=========================================================================================
	// compound assignment operator
	//=========================================================================================

	/* Add */
	Matrix4x4& operator+=(const Matrix4x4& mat) {
		for (int row = 0; row < 4; row++) {
			for (int column = 0; column < 4; column++) {
				m[row][column] += mat.m[row][column];
			}
		}

		return *this;
	}

	/* Subtract */
	Matrix4x4& operator-=(const Matrix4x4& mat) {
		for (int row = 0; row < 4; row++) {
			for (int column = 0; column < 4; column++) {
				m[row][column] -= mat.m[row][column];
			}
}

		return *this;
	}

	/* Multiply */
	Matrix4x4& operator*=(const Matrix4x4& mat) {
		Matrix4x4 result = { 0.0f };

		for (int row = 0; row < 4; row++) {
			for (int column = 0; column < 4; column++) {
				for (int i = 0; i < 4; i++) {
					result.m[row][column] += m[row][i] * mat.m[i][column];
				}
			}
		}

		std::memcpy(this, &result, sizeof(Matrix4x4));
		return *this;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// operators
////////////////////////////////////////////////////////////////////////////////////////////
//=========================================================================================
// binary operator
//=========================================================================================

/* Add */
inline Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = m1.m[row][column] + m2.m[row][column];
		}
	}

	return result;
}

/* Subtract */
inline Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] = m1.m[row][column] - m2.m[row][column];
		}
	}

	return result;
}

/* Multiply */
inline Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = { 0.0f };

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			for (int i = 0; i < 4; i++) {
				result.m[row][column] += m1.m[row][i] * m2.m[i][column];
			}
		}
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// enum RotateType
////////////////////////////////////////////////////////////////////////////////////////////
enum RotateType {
	kRotateBaseX,
	kRotateBaseY,
	kRotateBaseZ,
};

////////////////////////////////////////////////////////////////////////////////////////////
// Matrix methods
////////////////////////////////////////////////////////////////////////////////////////////
namespace Matrix {
	
	Matrix4x4 MakeScale(const Vector3f& scale);

	Matrix4x4 MakeRotate(float radian, RotateType type);
	Matrix4x4 MakeRotate(const Vector3f& rotate);

	Matrix4x4 MakeRotateEuler(float radian, RotateType type);
	Matrix4x4 MakeRotateEuler(const Vector3f& rotate);

	Matrix4x4 MakeTranslate(const Vector3f& translate);

	Matrix4x4 MakeAffine(const Vector3f& scale, const Vector3f& rotate, const Vector3f& translate);

	Matrix4x4 Inverse(const Matrix4x4& m);

	Matrix4x4 Transpose(const Matrix4x4& m);

	// graphics matrix //
	Matrix4x4 MakePerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);
	Matrix4x4 MakeOrthographic(float left, float top, float right, float bottom, float nearZ, float farZ);

	Matrix4x4 MakeViewport(float left, float top, float width, float height, float minDepth, float maxDepth);

	Vector3f Transform(const Vector3f& vector, const Matrix4x4& matrix);
}