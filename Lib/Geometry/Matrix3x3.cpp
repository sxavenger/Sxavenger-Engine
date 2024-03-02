#include "Matrix3x3.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Matrix methods
////////////////////////////////////////////////////////////////////////////////////////////

Matrix3x3 Matrix::MakeScale(const Vector2f& scale) {
	return {
		scale.x, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f,
		0.0f, 0.0f, 1.0f,
	};
}

Matrix3x3 Matrix::MakeRotate(float theta) {
	return {
		std::cos(theta), std::sin(theta), 0.0f,
		-std::sin(theta), std::cos(theta), 0.0f,
		0.0f, 0.0f, 1.0f,
	};
}

Matrix3x3 Matrix::MakeTranslate(const Vector2f& translate) {
	return {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		translate.x, translate.y, 1.0f,
	};
}

Matrix3x3 Matrix::MakeAffin(const Vector2f& scale, float theta, const Vector2f& translate) {
	return {
	scale.x * cosf(theta), scale.x * sinf(theta), 0.0f,
	scale.y * -sinf(theta), scale.y * cosf(theta), 0.0f,
	translate.x, translate.y, 1.0f,
	};
}

Matrix3x3 Matrix::Inverse(const Matrix3x3& matrix) {
	Matrix3x3 result{
		matrix.m[1][1] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][1],    // [0][0]
		-(matrix.m[0][1] * matrix.m[2][2] - matrix.m[0][2] * matrix.m[2][1]), // [0][1]
		matrix.m[0][1] * matrix.m[1][2] - matrix.m[0][2] * matrix.m[1][1],    // [0][2]

		-(matrix.m[1][0] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][0]), // [1][0]
		matrix.m[0][0] * matrix.m[2][2] - matrix.m[0][2] * matrix.m[2][0],    // [1][1]
		-(matrix.m[0][0] * matrix.m[1][2] - matrix.m[0][2] * matrix.m[1][0]), // [1][2]

		matrix.m[1][0] * matrix.m[2][1] - matrix.m[1][1] * matrix.m[2][0],    // [2][0]
		-(matrix.m[0][0] * matrix.m[2][1] - matrix.m[0][1] * matrix.m[2][0]), // [2][1]
		matrix.m[0][0] * matrix.m[1][1] - matrix.m[0][1] * matrix.m[1][0],    // [2][2]
	};

	float denominator = (matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][2])
		+ (matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][0])
		+ (matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][1])
		- (matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][0])
		- (matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][2])
		- (matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][1]);

	assert(denominator != 0.0f);

	for (int row = 0; row < 3; row++) {
		for (int column = 0; column < 3; column++) {
			result.m[row][column] *= 1.0f / denominator;
		}
	}

	return result;
}

Matrix3x3 Matrix3x3::MakeIdentity() {
	return {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
}
