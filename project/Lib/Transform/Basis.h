#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* math
#include "../Math/Vector3.h"
#include "../Math/Quaternion.h"

//* c++
#include <cstdint>
#include <array>
#include <initializer_list>

////////////////////////////////////////////////////////////////////////////////////////////
// Basis structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Basis {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Basis() noexcept = default;

	constexpr Basis(const Basis&) noexcept = default;
	constexpr Basis(Basis&&) noexcept = default;

	constexpr Basis(const std::initializer_list<float>& list) noexcept;
	constexpr Basis(const std::initializer_list<std::initializer_list<float>>& list) noexcept;

	//* operator *//

	constexpr Basis& operator=(const Basis&) noexcept = default;
	constexpr Basis& operator=(Basis&&) noexcept      = default;

	constexpr Basis& operator+=(const Basis& rhs) noexcept;

	constexpr Basis& operator-=(const Basis& rhs) noexcept;

	constexpr Basis& operator*=(const Basis& rhs) noexcept;

	//* mathmatical methods *//

	constexpr Basis Transpose() const noexcept;

	constexpr Basis Inverse() const noexcept;

	constexpr static Basis Identity() noexcept;

	static Basis MakeScale(float v) noexcept;
	static Basis MakeScale(const Vector3f& v) noexcept;

	static Basis MakeRotateX(float v) noexcept;
	static Basis MakeRotateY(float v) noexcept;
	static Basis MakeRotateZ(float v) noexcept;
	static Basis MakeRotateEuler(const Vector3f& euler) noexcept;
	static Basis MakeRotateQuaternion(const Quaternion& q) noexcept;

	constexpr static Vector3f TransformNormal(const Vector3f& v, const Basis& b) noexcept;

	//=========================================================================================
	// public variables
	//=========================================================================================

	std::array<std::array<float, 3>, 3> m = {};

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// Basis class constexpr methods
////////////////////////////////////////////////////////////////////////////////////////////

inline constexpr Basis::Basis(const std::initializer_list<float>& list) noexcept {
	for (size_t row = 0; row < 3; ++row) {
		for (size_t col = 0; col < 3; ++col) {
			m[row][col] = *(list.begin() + row * 3 + col);
		}
	}
}

inline constexpr Basis::Basis(const std::initializer_list<std::initializer_list<float>>& list) noexcept {
	for (size_t row = 0; row < 3; ++row) {
		for (size_t col = 0; col < 3; ++col) {
			m[row][col] = *(list.begin() + row)->begin() + col;
		}
	}
}

inline constexpr Basis& Basis::operator+=(const Basis& rhs) noexcept {
	for (size_t row = 0; row < 3; ++row) {
		for (size_t col = 0; col < 3; ++col) {
			m[row][col] += rhs.m[row][col];
		}
	}

	return *this;
}

inline constexpr Basis& Basis::operator-=(const Basis& rhs) noexcept {
	for (size_t row = 0; row < 3; ++row) {
		for (size_t col = 0; col < 3; ++col) {
			m[row][col] -= rhs.m[row][col];
		}
	}

	return *this;
}

inline constexpr Basis& Basis::operator*=(const Basis& rhs) noexcept {
	Basis v = {};

	for (size_t row = 0; row < 3; ++row) {
		for (size_t col = 0; col < 3; ++col) {
			for (size_t i = 0; i < 3; ++i) {
				v.m[row][col] += m[row][i] * rhs.m[i][col];
			}
		}
	}

	*this = v;
	return *this;
}

inline constexpr Basis Basis::Transpose() const noexcept {
	Basis v = {};

	for (size_t row = 0; row < 3; ++row) {
		for (size_t col = 0; col < 3; ++col) {
			v.m[row][col] = m[col][row];
		}
	}

	return v;
}

inline constexpr Basis Basis::Inverse() const noexcept {
	Basis v = {
		m[1][1] * m[2][2] - m[1][2] * m[2][1],    // [0][0]
		-(m[0][1] * m[2][2] - m[0][2] * m[2][1]), // [0][1]
		m[0][1] * m[1][2] - m[0][2] * m[1][1],    // [0][2]

		-(m[1][0] * m[2][2] - m[1][2] * m[2][0]), // [1][0]
		m[0][0] * m[2][2] - m[0][2] * m[2][0],    // [1][1]
		-(m[0][0] * m[1][2] - m[0][2] * m[1][0]), // [1][2]

		m[1][0] * m[2][1] - m[1][1] * m[2][0],    // [2][0]
		-(m[0][0] * m[2][1] - m[0][1] * m[2][0]), // [2][1]
		m[0][0] * m[1][1] - m[0][1] * m[1][0],    // [2][2]
	};

	float denominator = (m[0][0] * m[1][1] * m[2][2])
		+ (m[0][1] * m[1][2] * m[2][0])
		+ (m[0][2] * m[1][0] * m[2][1])
		- (m[0][2] * m[1][1] * m[2][0])
		- (m[0][1] * m[1][0] * m[2][2])
		- (m[0][0] * m[1][2] * m[2][1]);

	if (denominator == 0) { //!< 0除算対策
		return {};
	}

	for (size_t row = 0; row < 3; row++) {
		for (size_t column = 0; column < 3; column++) {
			v.m[row][column] *= 1.0f / denominator;
		}
	}

	return v;
}

constexpr Basis Basis::Identity() noexcept {
	return {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

inline constexpr Vector3f Basis::TransformNormal(const Vector3f& v, const Basis& b) noexcept {
	Vector3f x = {};

	for (size_t vi = 0; vi < 3; ++vi) {
		x[vi] = v[0] * b.m[0][vi] + v[1] * b.m[1][vi] + v[2] * b.m[2][vi];
	}

	return x;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Basis class binary operators
////////////////////////////////////////////////////////////////////////////////////////////

constexpr Basis operator+(const Basis& lhs, const Basis& rhs) noexcept {
	Basis v = {};

	for (size_t row = 0; row < 3; ++row) {
		for (size_t col = 0; col < 3; ++col) {
			v.m[row][col] = lhs.m[row][col] + rhs.m[row][col];
		}
	}

	return v;
}

constexpr Basis operator-(const Basis& lhs, const Basis& rhs) noexcept {
	Basis v = {};

	for (size_t row = 0; row < 3; ++row) {
		for (size_t col = 0; col < 3; ++col) {
			v.m[row][col] = lhs.m[row][col] - rhs.m[row][col];
		}
	}

	return v;
}

constexpr Basis operator*(const Basis& lhs, const Basis& rhs) noexcept {
	Basis v = {};

	for (size_t row = 0; row < 3; ++row) {
		for (size_t col = 0; col < 3; ++col) {
			for (size_t i = 0; i < 3; ++i) {
				v.m[row][col] += lhs.m[row][i] * rhs.m[i][col];
			}
		}
	}

	return v;
}
