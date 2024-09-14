#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cmath>
#include <cassert>
#include <Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Matrix3x3 class
////////////////////////////////////////////////////////////////////////////////////////////
class Matrix3x3 {
public:

	float m[3][3];

	static Matrix3x3 MakeIdentity();

	//=========================================================================================
	// operator
	//=========================================================================================

	/* Add */
	Matrix3x3 operator+(const Matrix3x3& other) const {
		Matrix3x3 result;

		for (int row = 0; row < 3; row++) {
			for (int column = 0; column < 3; column++) {
				result.m[row][column] = m[row][column] + other.m[row][column];
			}
		}

		return result;
	}
	void operator+=(const Matrix3x3& other) {
		for (int row = 0; row < 3; row++) {
			for (int column = 0; column < 3; column++) {
				m[row][column] += other.m[row][column];
			}
		}
	}

	/* Subtruct */
	Matrix3x3 operator-(const Matrix3x3& other) const {
		Matrix3x3 result;

		for (int row = 0; row < 3; row++) {
			for (int column = 0; column < 3; column++) {
				result.m[row][column] = m[row][column] - other.m[row][column];
			}
		}

		return result;
	}
	void operator-=(const Matrix3x3& other) {
		for (int row = 0; row < 3; row++) {
			for (int column = 0; column < 3; column++) {
				m[row][column] -= other.m[row][column];
			}
		}
	}

	/* Multiply */
	Matrix3x3 operator*(const Matrix3x3& other) const {
		Matrix3x3 result = { 0.0f };

		for (int row = 0; row < 3; row++) {
			for (int column = 0; column < 3; column++) {
				for (int i = 0; i < 3; i++) {
					result.m[row][column] += m[row][i] * other.m[i][column];
				}
			}
		}

		return result;
	}
	void operator*=(const Matrix3x3& other) {
		Matrix3x3 result = { 0.0f };

		for (int row = 0; row < 3; row++) {
			for (int column = 0; column < 3; column++) {
				for (int i = 0; i < 3; i++) {
					result.m[row][column] += m[row][i] * other.m[i][column];
				}
			}
		}


		for (int row = 0; row < 3; row++) {
			for (int column = 0; column < 3; column++) {
				m[row][column] = result.m[row][column];
			}
		}
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Matrix methods
////////////////////////////////////////////////////////////////////////////////////////////
namespace Matrix {

	Matrix3x3 MakeScale(const Vector2f& scale);

	Matrix3x3 MakeRotate(float theta);

	Matrix3x3 MakeTranslate(const Vector2f& translate);

	Matrix3x3 MakeAffin(const Vector2f& scale, float theta, const Vector2f& translate);

	Matrix3x3 Inverse(const Matrix3x3& matrix);

}