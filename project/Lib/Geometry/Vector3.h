#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Sxl/Formatter.h>

//* c++
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////
// Vector3 class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class Vector3 {
public:

	//=========================================================================================
	// constructor
	//=========================================================================================

	constexpr Vector3() = default;
	constexpr Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

	//=========================================================================================
	// compound assignment operator
	//=========================================================================================

	/* Add */
	constexpr Vector3& operator+=(const Vector3& rhs);

	/* Subtract */
	constexpr Vector3& operator-=(const Vector3& rhs);

	/* Multiply */
	constexpr Vector3& operator*=(const Vector3& rhs);
	constexpr Vector3& operator*=(T rhs);

	/* Division */
	constexpr Vector3& operator/=(const Vector3& rhs);
	constexpr Vector3& operator/=(T rhs);

	//=========================================================================================
	// cast opraotor
	//=========================================================================================

	template <typename U>
	constexpr operator Vector3<U>() const {
		return { static_cast<U>(x), static_cast<U>(y), static_cast<U>(z) };
	}

	//=========================================================================================
	// random access operator
	//=========================================================================================

	constexpr T& operator[](size_t i) {
		return (&x)[i];
	}

	const T& operator[](size_t i) const {
		return (&x)[i];
	}

	//=========================================================================================
	// variables
	//=========================================================================================

	T x, y, z;

	//=========================================================================================
	// formatter
	//=========================================================================================

	template <typename FormatContext>
	auto format(FormatContext& ctx) const {
		return std::format_to(ctx.out(), "(x: {}, y: {}, z: {})", x, y, z);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Vector3 class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
constexpr Vector3<T>& Vector3<T>::operator+=(const Vector3<T>& rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

template <typename T>
constexpr Vector3<T>& Vector3<T>::operator-=(const Vector3<T>& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

template <typename T>
constexpr Vector3<T>& Vector3<T>::operator*=(const Vector3<T>& rhs) {
	x *= rhs.x;
	y *= rhs.y;
	z *= rhs.z;
	return *this;
}

template <typename T>
constexpr Vector3<T>& Vector3<T>::operator*=(T rhs) {
	x *= rhs;
	y *= rhs;
	z *= rhs;
	return *this;
}

template <typename T>
constexpr Vector3<T>& Vector3<T>::operator/=(const Vector3<T>& rhs) {
	x /= rhs.x;
	y /= rhs.y;
	z /= rhs.z;
	return *this;
}

template <typename T>
constexpr Vector3<T>& Vector3<T>::operator/=(T rhs) {
	x /= rhs;
	y /= rhs;
	z /= rhs;
	return *this;
}

//=========================================================================================
// binary operator
//=========================================================================================

/* Add */
template <typename T>
constexpr Vector3<T> operator+(const Vector3<T>& lhs, const Vector3<T>& rhs) {
	return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}

/* Subtract */
template <typename T>
constexpr Vector3<T> operator-(const Vector3<T>& lhs, const Vector3<T>& rhs) {
	return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}

/* Multiply */
template <typename T>
constexpr Vector3<T> operator*(const Vector3<T>& lhs, const Vector3<T>& rhs) {
	return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
}

template <typename T>
constexpr Vector3<T> operator*(const Vector3<T>& lhs, T rhs) {
	return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
}

template <typename T>
constexpr Vector3<T> operator*(T lhs, const Vector3<T>& rhs) {
	return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z };
}

/* Division */
template <typename T>
constexpr Vector3<T> operator/(const Vector3<T>& lhs, const Vector3<T>& rhs) {
	return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
}

template <typename T>
constexpr Vector3<T> operator/(const Vector3<T>& lhs, T rhs) {
	return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs };
}

template <typename T>
constexpr Vector3<T> operator/(T lhs, const Vector3<T>& rhs) {
	return { lhs / rhs.x, lhs / rhs.y, lhs / rhs.z };
}

//=========================================================================================
// unary operator
//=========================================================================================

template <typename T>
constexpr Vector3<T> operator+(const Vector3<T>& v) {
	return v;
}

template <typename T>
constexpr Vector3<T> operator-(const Vector3<T>& v) {
	return { -v.x, -v.y, -v.z };
}

////////////////////////////////////////////////////////////////////////////////////////////
// formatter
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
struct std::formatter<Vector3<T>> : Sxl::BaseFormatter<Vector3<T>> {};

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------

using Vector3f = Vector3<float>;
using Vector3ui = Vector3<uint32_t>;

//-----------------------------------------------------------------------------------------
// constexpr
//-----------------------------------------------------------------------------------------

template <class T = float>
constexpr const Vector3<T> kOrigin3 = { 0, 0, 0 };

template <class T = float>
constexpr const Vector3<T> kUnit3 = { 1, 1, 1 };
