#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* geometry
#include "Vector2.h"
#include "Vector3.h"

//* c++
#include <cstdint>
#include <concepts>
#include <limits>
#include <cmath>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////
// Vector4 class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Ty>
class Vector4 {
public:

	//=========================================================================================
	// constructor
	//=========================================================================================

	constexpr Vector4() noexcept = default;
	constexpr Vector4(_Ty _x, _Ty _y, _Ty _z, _Ty _w) noexcept : x(_x), y(_y), z(_z), w(_w) {};
	constexpr Vector4(const Vector4& rhs) noexcept = default;
	constexpr Vector4(Vector4&& rhs) noexcept = default;

	constexpr Vector4(const Vector2<_Ty>& rhs, _Ty _z, _Ty _w) noexcept : x(rhs.x), y(rhs.y), z(_z), w(_w) {};
	constexpr Vector4(const Vector3<_Ty>& rhs, _Ty _w) noexcept : x(rhs.x), y(rhs.y), z(rhs.z), w(_w) {};

	//=========================================================================================
	// operators
	//=========================================================================================

	//* compound assignment
	constexpr Vector4& operator=(const Vector4& rhs) noexcept = default;
	constexpr Vector4& operator=(Vector4&& rhs) noexcept = default;
	constexpr Vector4& operator+=(const Vector4& rhs) noexcept;
	constexpr Vector4& operator-=(const Vector4& rhs) noexcept;
	constexpr Vector4& operator*=(const Vector4& rhs) noexcept;
	constexpr Vector4& operator*=(const _Ty& rhs) noexcept;
	constexpr Vector4& operator/=(const Vector4& rhs) noexcept;
	constexpr Vector4& operator/=(const _Ty& rhs) noexcept;

	//* cast
	template <class _U>
	constexpr operator Vector4<_U>() const noexcept {
		return { static_cast<_U>(x), static_cast<_U>(y), static_cast<_U>(z), static_cast<_U>(w) };
	}

	//* unary
	constexpr Vector4 operator+() const noexcept;
	constexpr Vector4 operator-() const noexcept;

	//* access
	constexpr _Ty& operator[](size_t index) noexcept;
	constexpr const _Ty& operator[](size_t index) const noexcept;

	//=========================================================================================
	// variables
	//=========================================================================================

	_Ty x, y, z, w;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Vector4 class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class _Ty>
constexpr Vector4<_Ty>& Vector4<_Ty>::operator+=(const Vector4& rhs) noexcept {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	w += rhs.w;
	return *this;
}

template <class _Ty>
constexpr Vector4<_Ty>& Vector4<_Ty>::operator-=(const Vector4& rhs) noexcept {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	w -= rhs.w;
	return *this;
}

template <class _Ty>
constexpr Vector4<_Ty>& Vector4<_Ty>::operator*=(const Vector4& rhs) noexcept {
	x *= rhs.x;
	y *= rhs.y;
	z *= rhs.z;
	w *= rhs.w;
	return *this;
}

template <class _Ty>
constexpr Vector4<_Ty>& Vector4<_Ty>::operator*=(const _Ty& rhs) noexcept {
	x *= rhs;
	y *= rhs;
	z *= rhs;
	w *= rhs;
	return *this;
}

template <class _Ty>
constexpr Vector4<_Ty>& Vector4<_Ty>::operator/=(const Vector4& rhs) noexcept {
	x /= rhs.x;
	y /= rhs.y;
	z /= rhs.z;
	w /= rhs.w;
	return *this;
}

template <class _Ty>
constexpr Vector4<_Ty>& Vector4<_Ty>::operator/=(const _Ty& rhs) noexcept {
	x /= rhs;
	y /= rhs;
	z /= rhs;
	w /= rhs;
	return *this;
}

template <class _Ty>
constexpr Vector4<_Ty> Vector4<_Ty>::operator+() const noexcept {
	return *this;
}

template <class _Ty>
constexpr Vector4<_Ty> Vector4<_Ty>::operator-() const noexcept {
	return { -x, -y, -z, -w };
}

template <class _Ty>
constexpr _Ty& Vector4<_Ty>::operator[](size_t index) noexcept {
	return (&x)[index];
}

template <class _Ty>
constexpr const _Ty& Vector4<_Ty>::operator[](size_t index) const noexcept {
	return (&x)[index];
}

////////////////////////////////////////////////////////////////////////////////////////////
// Vector4 class binary operators
////////////////////////////////////////////////////////////////////////////////////////////

template <class _Ty>
constexpr Vector4<_Ty> operator+(const Vector4<_Ty>& lhs, const Vector4<_Ty>& rhs) noexcept {
	return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
}

template <class _Ty>
constexpr Vector4<_Ty> operator-(const Vector4<_Ty>& lhs, const Vector4<_Ty>& rhs) noexcept {
	return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
}

template <class _Ty>
constexpr Vector4<_Ty> operator*(const Vector4<_Ty>& lhs, const Vector4<_Ty>& rhs) noexcept {
	return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w };
}

template <class _Ty>
constexpr Vector4<_Ty> operator*(const Vector4<_Ty>& lhs, const _Ty& rhs) noexcept {
	return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs };
}

template <class _Ty>
constexpr Vector4<_Ty> operator*(const _Ty& lhs, const Vector4<_Ty>& rhs) noexcept {
	return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w };
}

template <class _Ty>
constexpr Vector4<_Ty> operator/(const Vector4<_Ty>& lhs, const Vector4<_Ty>& rhs) noexcept {
	return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w };
}

template <class _Ty>
constexpr Vector4<_Ty> operator/(const Vector4<_Ty>& lhs, const _Ty& rhs) noexcept {
	return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs };
}

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

using Vector4i  = Vector4<std::int32_t>;
using Vector4ui = Vector4<std::uint32_t>;
using Vector4f  = Vector4<float>;

