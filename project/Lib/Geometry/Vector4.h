#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Sxl/Formatter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Vector4 class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class Vector4 {
public:

	//=========================================================================================
	// constructor
	//=========================================================================================

	constexpr Vector4() = default;
	constexpr Vector4(T _x, T _y, T _z) : x(_x), y(_y), z(_z), w(1.0f) {}
	constexpr Vector4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}

	//=========================================================================================
	// compound assignment operator
	//=========================================================================================

	/* Add */
	constexpr Vector4& operator+=(const Vector4& rhs);

	/* Subtract */
	constexpr Vector4& operator-=(const Vector4& rhs);

	/* Multiply */
	constexpr Vector4& operator*=(const Vector4& rhs);
	constexpr Vector4& operator*=(T rhs);

	/* Division */
	constexpr Vector4& operator/=(const Vector4& rhs);
	constexpr Vector4& operator/=(T rhs);

	//=========================================================================================
	// cast opraotor
	//=========================================================================================

	template <typename U>
	constexpr operator Vector4<U>() const {
		return { static_cast<U>(x), static_cast<U>(y), static_cast<U>(z), static_cast<U>(w) };
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

	T x, y, z, w;

	//=========================================================================================
	// formatter
	//=========================================================================================

	template <typename FormatContext>
	auto format(FormatContext& ctx) const {
		return std::format_to(ctx.out(), "(x: {}, y: {}, z: {}, w: {})", x, y, z, w);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Vector4 class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
constexpr Vector4<T>& Vector4<T>::operator+=(const Vector4& rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	w += rhs.w;
	return *this;
}

template <typename T>
constexpr Vector4<T>& Vector4<T>::operator-=(const Vector4& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	w -= rhs.w;
	return *this;
}

template <typename T>
constexpr Vector4<T>& Vector4<T>::operator*=(const Vector4& rhs) {
	x *= rhs.x;
	y *= rhs.y;
	z *= rhs.z;
	w *= rhs.w;
	return *this;
}

template <typename T>
constexpr Vector4<T>& Vector4<T>::operator*=(T rhs) {
	x *= rhs;
	y *= rhs;
	z *= rhs;
	w *= rhs;
	return *this;
}

template <typename T>
constexpr Vector4<T>& Vector4<T>::operator/=(const Vector4& rhs) {
	x /= rhs.x;
	y /= rhs.y;
	z /= rhs.z;
	w /= rhs.w;
	return *this;
}

template <typename T>
constexpr Vector4<T>& Vector4<T>::operator/=(T rhs) {
	x /= rhs;
	y /= rhs;
	z /= rhs;
	w /= rhs;
	return *this;
}

//=========================================================================================
// binary operator
//=========================================================================================

/* Add */
template <typename T>
constexpr Vector4<T> operator+(const Vector4<T>& lhs, const Vector4<T>& rhs) {
	return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
}

/* Subtract */
template <typename T>
constexpr Vector4<T> operator-(const Vector4<T>& lhs, const Vector4<T>& rhs) {
	return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
}

/* Multiply */
template <typename T>
constexpr Vector4<T> operator*(const Vector4<T>& lhs, const Vector4<T>& rhs) {
	return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w };
}

template <typename T>
constexpr Vector4<T> operator*(const Vector4<T>& lhs, T rhs) {
	return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs };
}

template <typename T>
constexpr Vector4<T> operator*(T lhs, const Vector4<T>& rhs) {
	return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w };
}

/* Division */
template <typename T>
constexpr Vector4<T> operator/(const Vector4<T>& lhs, const Vector4<T>& rhs) {
	return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w };
}

template <typename T>
constexpr Vector4<T> operator/(const Vector4<T>& lhs, T rhs) {
	return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs };
}

template <typename T>
constexpr Vector4<T> operator/(T lhs, const Vector4<T>& rhs) {
	return { lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w };
}

//=========================================================================================
// unary operator
//=========================================================================================

template <typename T>
constexpr Vector4<T> operator+(const Vector4<T>& v) {
	return v;
}

template <typename T>
constexpr Vector4<T> operator-(const Vector4<T>& v) {
	return { -v.x, -v.y, -v.z, -v.w };
}

////////////////////////////////////////////////////////////////////////////////////////////
// formatter
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
struct std::formatter<Vector4<T>> : Sxl::BaseFormatter<Vector4<T>> {};

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------

using Vector4f = Vector4<float>;
using Vector4i = Vector4<int32_t>;

