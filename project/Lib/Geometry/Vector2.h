#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Sxl/Formatter.h>

//* c++
#include <cstdint>
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// Vector2 class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class Vector2 {
public:

	//=========================================================================================
	// constructor
	//=========================================================================================

	constexpr Vector2() = default;
	constexpr Vector2(T _x, T _y) : x(_x), y(_y) {}

	//=========================================================================================
	// compound assignment operator
	//=========================================================================================

	/* Add */
	constexpr Vector2& operator+=(const Vector2& rhs);

	/* Subtract */
	constexpr Vector2& operator-=(const Vector2& rhs);

	/* Multiply */
	constexpr Vector2& operator*=(const Vector2& rhs);
	constexpr Vector2& operator*=(T rhs);

	/* Division */
	constexpr Vector2& operator/=(const Vector2& rhs);
	constexpr Vector2& operator/=(T rhs);

	//=========================================================================================
	// cast operator
	//=========================================================================================

	template <typename U>
	constexpr operator Vector2<U>() const {
		return { static_cast<U>(x), static_cast<U>(y) };
	}

	//=========================================================================================
	// access operator
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

	T x, y;

	//=========================================================================================
	// formatter
	//=========================================================================================

	template <typename FormatContext>
	auto format(FormatContext& ctx) const {
		return std::format_to(ctx.out(), "(x: {}, y: {})", x, y);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Vector2 class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline constexpr Vector2<T>& Vector2<T>::operator+=(const Vector2& rhs) {
	x += rhs.x; y += rhs.y;
	return *this;
}

template <typename T>
inline constexpr Vector2<T>& Vector2<T>::operator-=(const Vector2& rhs) {
	x -= rhs.x; y -= rhs.y;
	return *this;
}

template <typename T>
inline constexpr Vector2<T>& Vector2<T>::operator*=(const Vector2& rhs) {
	x *= rhs.x; y *= rhs.y;
	return *this;
}

template <typename T>
inline constexpr Vector2<T>& Vector2<T>::operator*=(T rhs) {
	x *= rhs; y *= rhs;
	return *this;
}

template <typename T>
inline constexpr Vector2<T>& Vector2<T>::operator/=(const Vector2& rhs) {
	x /= rhs.x; y /= rhs.y;
	return *this;
}

template <typename T>
inline constexpr Vector2<T>& Vector2<T>::operator/=(T rhs) {
	x /= rhs; y /= rhs;
	return *this;
}

//=========================================================================================
// binary operator
//=========================================================================================

/* Add */
template <typename T>
constexpr Vector2<T> operator+(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return { lhs.x + rhs.x, lhs.y + rhs.y };
}

/* Subtract */
template <typename T>
constexpr Vector2<T> operator-(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return { lhs.x - rhs.x, lhs.y - rhs.y };
}

/* Multiply */
template <typename T>
constexpr Vector2<T> operator*(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return { lhs.x * rhs.x, lhs.y * rhs.y };
}

template <typename T>
constexpr Vector2<T> operator*(const Vector2<T>& lhs, T rhs) {
	return { lhs.x * rhs, lhs.y * rhs };
}

template <typename T>
constexpr Vector2<T> operator*(T lhs, const Vector2<T>& rhs) {
	return { lhs * rhs.x, lhs * rhs.y };
}

/* Division */
template <typename T>
constexpr Vector2<T> operator/(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return { lhs.x / rhs.x, lhs.y / rhs.y };
}

template <typename T>
constexpr Vector2<T> operator/(const Vector2<T>& lhs, T rhs) {
	return { lhs.x / rhs, lhs.y / rhs };
}

//=========================================================================================
// unary operator
//=========================================================================================

template <typename T>
constexpr Vector2<T> operator+(const Vector2<T> v) {
	return v;
}

template <typename T>
constexpr Vector2<T> operator-(const Vector2<T> v) {
	return { -v.x, -v.y };
}


////////////////////////////////////////////////////////////////////////////////////////////
// formatter
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
struct std::formatter<Vector2<T>> : Sxl::BaseFormatter<Vector2<T>> {};

//-----------------------------------------------------------------------------------------
// constexpr
//-----------------------------------------------------------------------------------------

template <class T = float>
constexpr const Vector2<T> kOrigin2 = { 0, 0 };

template <class T = float>
constexpr const Vector2<T> kUnit2 = { 1, 1 };

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------

using Vector2f  = Vector2<float>;
using Vector2i  = Vector2<int32_t>;
using Vector2ui = Vector2<uint32_t>;
