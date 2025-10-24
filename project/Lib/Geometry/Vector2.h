#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include <Lib/Sxl/Formatter.h>

//* c++
#include <cstdint>
#include <concepts>
#include <limits>
#include <cmath>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////
// Vector2 class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Ty>
class Vector2 {
public:

	//=========================================================================================
	// constructor
	//=========================================================================================

	constexpr Vector2() noexcept = default;
	constexpr Vector2(_Ty _x, _Ty _y) noexcept : x(_x), y(_y) {};
	constexpr Vector2(const Vector2& rhs) noexcept = default;
	constexpr Vector2(Vector2&& rhs) noexcept = default;

	//=========================================================================================
	// operators
	//=========================================================================================

	//* compound assignment
	constexpr Vector2& operator=(const Vector2& rhs) noexcept = default;
	constexpr Vector2& operator=(Vector2&& rhs) noexcept = default;
	constexpr Vector2& operator+=(const Vector2& rhs) noexcept { x += rhs.x; y += rhs.y; return *this; }
	constexpr Vector2& operator-=(const Vector2& rhs) noexcept { x -= rhs.x; y -= rhs.y; return *this; }
	constexpr Vector2& operator*=(const Vector2& rhs) noexcept { x *= rhs.x; y *= rhs.y; return *this; }
	constexpr Vector2& operator*=(const _Ty rhs) noexcept { x *= rhs; y *= rhs; return *this; }
	constexpr Vector2& operator/=(const Vector2& rhs) noexcept { x /= rhs.x; y /= rhs.y; return *this; }
	constexpr Vector2& operator/=(const _Ty rhs) noexcept { x /= rhs; y /= rhs; return *this; }

	//* cast
	template <class _U>
	constexpr operator Vector2<_U>() const noexcept { return { static_cast<_U>(x), static_cast<_U>(y) }; }

	//* unary
	constexpr Vector2 operator+() const noexcept { return *this; }
	constexpr Vector2 operator-() const noexcept { return { -x, -y }; }

	//* access
	constexpr _Ty& operator[](size_t index) noexcept { return (&x)[index]; }
	constexpr const _Ty& operator[](size_t index) const noexcept { return (&x)[index]; }

	//=========================================================================================
	// variables
	//=========================================================================================

	_Ty x, y;

	//=========================================================================================
	// formatter
	//=========================================================================================

	template <typename FormatContext>
	auto format(FormatContext& ctx) const {
		return std::format_to(ctx.out(), "({}, {})", x, y);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Vector2 - floating_point class
////////////////////////////////////////////////////////////////////////////////////////////
template <std::floating_point _Ty>
class Vector2<_Ty> {
public:

	//=========================================================================================
	// constructor
	//=========================================================================================

	constexpr Vector2() noexcept = default;
	constexpr Vector2(_Ty _x, _Ty _y) noexcept : x(_x), y(_y) {};
	constexpr Vector2(const Vector2& rhs) noexcept = default;

	//=========================================================================================
	// operator
	//=========================================================================================

	//* compound assignment
	constexpr Vector2& operator=(const Vector2& rhs) noexcept = default;
	constexpr Vector2& operator=(Vector2&& rhs) noexcept = default;
	constexpr Vector2& operator+=(const Vector2& rhs) noexcept { x += rhs.x; y += rhs.y; return *this; }
	constexpr Vector2& operator-=(const Vector2& rhs) noexcept { x -= rhs.x; y -= rhs.y; return *this; }
	constexpr Vector2& operator*=(const Vector2& rhs) noexcept { x *= rhs.x; y *= rhs.y; return *this; }
	constexpr Vector2& operator*=(const _Ty rhs) noexcept { x *= rhs; y *= rhs; return *this; }
	constexpr Vector2& operator/=(const Vector2& rhs) noexcept { x /= rhs.x; y /= rhs.y; return *this; }
	constexpr Vector2& operator/=(const _Ty rhs) noexcept { x /= rhs; y /= rhs; return *this; }

	//* cast
	template <class _U>
	operator Vector2<_U>() const noexcept { return { static_cast<_U>(x), static_cast<_U>(y) }; }

	//* unary
	constexpr Vector2 operator+() const noexcept { return *this; }
	constexpr Vector2 operator-() const noexcept { return { -x, -y }; }

	//* access
	constexpr _Ty& operator[](size_t index) noexcept { return (&x)[index]; }
	constexpr const _Ty& operator[](size_t index) const noexcept { return (&x)[index]; }

	//=========================================================================================
	// variables
	//=========================================================================================

	_Ty x, y;

	//=========================================================================================
	// mathmatical methods
	//=========================================================================================

	Vector2 Saturate() const {
		return { std::clamp(x, (_Ty)0, (_Ty)1), std::clamp(y, (_Ty)0, (_Ty)1) };
	}

	Vector2 Abs() const {
		return { std::abs(x), std::abs(y) };
	}

	_Ty Length() const {
		return std::sqrt(x * x + y * y);
	}

	Vector2 Normalize() const {
		return Length() == 0.0f ? Vector2() : *this / Length();
	}

	static _Ty Dot(const Vector2& lhs, const Vector2& rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	static _Ty Cross(const Vector2& lhs, const Vector2& rhs) {
		return lhs.x * rhs.y - lhs.y * rhs.x;
	}

	static Vector2 Lerp(const Vector2& x, const Vector2& y, _Ty t) {
		return x * (1.0f - t) + y * t;
	}

	static Vector2 Reflect(const Vector2& v, const Vector2& n) {
		return v - n * (2.0f * Dot(v, n));
	}

	static Vector2 Clamp(const Vector2& v, const Vector2& min, const Vector2& max) {
		return { std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y) };
	}

	static Vector2 Min(const Vector2& lhs, const Vector2& rhs) {
		return { std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y) };
	}

	static Vector2 Max(const Vector2& lhs, const Vector2& rhs) {
		return { std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y) };
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Vector2 class binary operators
////////////////////////////////////////////////////////////////////////////////////////////

template <class _Ty>
constexpr Vector2<_Ty> operator+(const Vector2<_Ty>& lhs, const Vector2<_Ty>& rhs) noexcept {
	return { lhs.x + rhs.x, lhs.y + rhs.y };
}

template <class _Ty>
constexpr Vector2<_Ty> operator-(const Vector2<_Ty>& lhs, const Vector2<_Ty>& rhs) noexcept {
	return { lhs.x - rhs.x, lhs.y - rhs.y };
}

template <class _Ty>
constexpr Vector2<_Ty> operator*(const Vector2<_Ty>& lhs, const Vector2<_Ty>& rhs) noexcept {
	return { lhs.x * rhs.x, lhs.y * rhs.y };
}

template <class _Ty>
constexpr Vector2<_Ty> operator*(const Vector2<_Ty>& lhs, const _Ty& rhs) noexcept {
	return { lhs.x * rhs, lhs.y * rhs };
}

template <class _Ty>
constexpr Vector2<_Ty> operator*(const _Ty& lhs, const Vector2<_Ty>& rhs) noexcept {
	return { lhs * rhs.x, lhs * rhs.y };
}

template <class _Ty>
constexpr Vector2<_Ty> operator/(const Vector2<_Ty>& lhs, const Vector2<_Ty>& rhs) noexcept {
	return { lhs.x / rhs.x, lhs.y / rhs.y };
}

template <class _Ty>
constexpr Vector2<_Ty> operator/(const Vector2<_Ty>& lhs, const _Ty& rhs) noexcept {
	return { lhs.x / rhs, lhs.y / rhs };
}

////////////////////////////////////////////////////////////////////////////////////////////
// Vector2 formatter structure
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
struct std::formatter<Vector2<T>> : Sxl::BaseFormatter<Vector2<T>> {};

////////////////////////////////////////////////////////////////////////////////////////////
// constexpr constants
////////////////////////////////////////////////////////////////////////////////////////////

template <class _Ty = float>
constexpr Vector2<_Ty> kOrigin2 = { 0, 0 };

template <class _Ty = float>
constexpr Vector2<_Ty> kUnit2 = { 1, 1 };

template <class _Ty = float>
constexpr Vector2<_Ty> kUnitX2 = { 1, 0 };

template <class _Ty = float>
constexpr Vector2<_Ty> kUnitY2 = { 0, 1 };

template <class _Ty = float>
constexpr Vector2<_Ty> kInfinity2 = { std::numeric_limits<_Ty>::infinity(), std::numeric_limits<_Ty>::infinity() };

////////////////////////////////////////////////////////////////////////////////////////////
// constexpr constants
////////////////////////////////////////////////////////////////////////////////////////////

using Vector2i  = Vector2<std::int32_t>;
using Vector2ui = Vector2<std::uint32_t>;
using Vector2uz = Vector2<std::size_t>;

using Vector2f = Vector2<float>;
