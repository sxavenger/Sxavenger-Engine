#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* geometry
#include "Vector3.h"
#include "Quaternion.h"

//* c++
#include <cstdint>
#include <array>
#include <initializer_list>

////////////////////////////////////////////////////////////////////////////////////////////
// Matrix4x4 class
////////////////////////////////////////////////////////////////////////////////////////////
class Matrix4x4 {
public:

	//=========================================================================================
	// constructor
	//=========================================================================================

	constexpr Matrix4x4() noexcept = default;
	constexpr Matrix4x4(const Matrix4x4& rhs) noexcept = default;
	constexpr Matrix4x4(Matrix4x4&& rhs) noexcept = default;

	constexpr Matrix4x4(const std::initializer_list<float>& list) noexcept;
	constexpr Matrix4x4(const std::initializer_list<std::initializer_list<float>>& list) noexcept;

	//=========================================================================================
	// operator
	//=========================================================================================

	//* compound assignment
	constexpr Matrix4x4& operator=(const Matrix4x4& rhs) noexcept = default;
	constexpr Matrix4x4& operator=(Matrix4x4&& rhs) noexcept = default;
	constexpr Matrix4x4& operator+=(const Matrix4x4& rhs) noexcept;
	constexpr Matrix4x4& operator-=(const Matrix4x4& rhs) noexcept;
	constexpr Matrix4x4& operator*=(const Matrix4x4& rhs) noexcept;

	//* access
	//!< c++23以降はoperator[]を作成

	//=========================================================================================
	// variables
	//=========================================================================================

	std::array<std::array<float, 4>, 4> m = {};

	//=========================================================================================
	// mathmatical methods
	//=========================================================================================

	//* member methods

	Matrix4x4 Transpose() const noexcept;

	Matrix4x4 Inverse() const noexcept;

	//* static member methods

	static Matrix4x4 Identity() noexcept;

	static Matrix4x4 MakeTranslate(const Vector3f& v) noexcept;

	static Matrix4x4 MakeRotateX(float angle) noexcept;
	static Matrix4x4 MakeRotateY(float angle) noexcept;
	static Matrix4x4 MakeRotateZ(float angle) noexcept;
	static Matrix4x4 MakeRotate(const Vector3f& euler) noexcept;
	static Matrix4x4 MakeRotate(const Quaternion& q) noexcept;

	static Matrix4x4 MakeScale(const Vector3f& v) noexcept;

	static Matrix4x4 MakeAffine(const Vector3f& scale, const Vector3f& rotate, const Vector3f& translate) noexcept;
	static Matrix4x4 MakeAffine(const Vector3f& scale, const Quaternion& rotate, const Vector3f& translate) noexcept;


	static Matrix4x4 PerspectiveFov(float fov, float aspect, float near, float far) noexcept;
	static Matrix4x4 Orthographic(float left, float top, float right, float bottom, float near, float far) noexcept;
	static Matrix4x4 Viewport(float x, float y, float width, float height, float near, float far) noexcept;


	static Vector3f Transform(const Vector3f& v, const Matrix4x4& m) noexcept;
	static Vector3f TransformNormal(const Vector3f& v, const Matrix4x4& m) noexcept;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Matrix4x4 class constexpr methods
////////////////////////////////////////////////////////////////////////////////////////////

constexpr Matrix4x4::Matrix4x4(const std::initializer_list<float>& list) noexcept {
	for (size_t row = 0; row < 4; ++row) {
		for (size_t col = 0; col < 4; ++col) {
			m[row][col] = *(list.begin() + row * 4 + col);
		}
	}
}

constexpr Matrix4x4::Matrix4x4(const std::initializer_list<std::initializer_list<float>>& list) noexcept {
	for (size_t row = 0; row < 4; ++row) {
		for (size_t col = 0; col < 4; ++col) {
			m[row][col] = *(list.begin() + row)->begin() + col;
		}
	}
}

constexpr Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& rhs) noexcept {
	for (size_t row = 0; row < 4; ++row) {
		for (size_t col = 0; col < 4; ++col) {
			m[row][col] += rhs.m[row][col];
		}
	}

	return *this;
}

constexpr Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& rhs) noexcept {
	for (size_t row = 0; row < 4; ++row) {
		for (size_t col = 0; col < 4; ++col) {
			m[row][col] -= rhs.m[row][col];
		}
	}

	return *this;
}

constexpr Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& rhs) noexcept {
	Matrix4x4 result = {};

	for (size_t row = 0; row < 4; ++row) {
		for (size_t col = 0; col < 4; ++col) {
			for (size_t i = 0; i < 4; ++i) {
				result.m[row][col] += m[row][i] * rhs.m[i][col];
			}
		}
	}

	*this = result;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Matrix4x4 class binary operators
////////////////////////////////////////////////////////////////////////////////////////////

constexpr Matrix4x4 operator+(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept {
	Matrix4x4 result = {};

	for (size_t row = 0; row < 4; ++row) {
		for (size_t col = 0; col < 4; ++col) {
			result.m[row][col] = lhs.m[row][col] + rhs.m[row][col];
		}
	}

	return result;
}

constexpr Matrix4x4 operator-(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept {
	Matrix4x4 result = {};

	for (size_t row = 0; row < 4; ++row) {
		for (size_t col = 0; col < 4; ++col) {
			result.m[row][col] = lhs.m[row][col] - rhs.m[row][col];
		}
	}

	return result;
}

constexpr Matrix4x4 operator*(const Matrix4x4& lhs, const Matrix4x4& rhs) noexcept {
	Matrix4x4 result = {};

	for (size_t row = 0; row < 4; ++row) {
		for (size_t col = 0; col < 4; ++col) {
			for (size_t i = 0; i < 4; ++i) {
				result.m[row][col] += lhs.m[row][i] * rhs.m[i][col];
			}
		}
	}

	return result;
}
