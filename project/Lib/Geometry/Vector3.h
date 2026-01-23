#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* geometry
#include "Vector2.h"

//* sxl
#include <Lib/Sxl/Formatter.h>

//* c++
#include <cstdint>
#include <concepts>
#include <limits>
#include <cmath>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////
// Vector3 class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Ty>
class Vector3 {
public:

	//=========================================================================================
	// constructor
	//=========================================================================================

	constexpr Vector3() noexcept = default;
	constexpr Vector3(_Ty _x, _Ty _y, _Ty _z) noexcept : x(_x), y(_y), z(_z) {};
	constexpr Vector3(const Vector3& rhs) noexcept = default;
	constexpr Vector3(Vector3&& rhs) noexcept = default;

	constexpr Vector3(const Vector2<_Ty>& rhs, _Ty _z) noexcept : x(rhs.x), y(rhs.y), z(_z) {};

	//=========================================================================================
	// operators
	//=========================================================================================

	//* compound assignment
	constexpr Vector3& operator=(const Vector3& rhs) noexcept = default;
	constexpr Vector3& operator=(Vector3&& rhs) noexcept = default;
	constexpr Vector3& operator+=(const Vector3& rhs) noexcept { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	constexpr Vector3& operator-=(const Vector3& rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	constexpr Vector3& operator*=(const Vector3& rhs) noexcept { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
	constexpr Vector3& operator*=(const _Ty rhs) noexcept { x *= rhs; y *= rhs; z *= rhs; return *this; }
	constexpr Vector3& operator/=(const Vector3& rhs) noexcept { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }
	constexpr Vector3& operator/=(const _Ty rhs) noexcept { x /= rhs; y /= rhs; z /= rhs; return *this; }

	//* cast
	template <class _U>
	constexpr operator Vector3<_U>() const noexcept { return { static_cast<_U>(x), static_cast<_U>(y), static_cast<_U>(z) }; }

	//* unary
	constexpr Vector3 operator+() const noexcept { return *this; }
	constexpr Vector3 operator-() const noexcept { return { -x, -y, -z }; }

	//* access
	constexpr _Ty& operator[](size_t index) noexcept { return (&x)[index]; }
	constexpr const _Ty& operator[](size_t index) const noexcept { return (&x)[index]; }

	//=========================================================================================
	// variables
	//=========================================================================================

	_Ty x, y, z;

	//=========================================================================================
	// formatter
	//=========================================================================================

	template <typename FormatContext>
	auto Format(FormatContext& ctx) const {
		return std::format_to(ctx.out(), "({}, {}, {})", x, y, z);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Vector3 - floating_point class
////////////////////////////////////////////////////////////////////////////////////////////
template <std::floating_point _Ty>
class Vector3<_Ty> {
public:

	//=========================================================================================
	// constructor
	//=========================================================================================

	constexpr Vector3() noexcept = default;
	constexpr Vector3(_Ty _x, _Ty _y, _Ty _z) noexcept : x(_x), y(_y), z(_z) {};
	constexpr Vector3(const Vector3& rhs) noexcept = default;

	constexpr Vector3(const Vector2<_Ty>& rhs, _Ty _z) noexcept : x(rhs.x), y(rhs.y), z(_z) {};

	//=========================================================================================
	// operator
	//=========================================================================================

	//* compound assignment
	constexpr Vector3& operator=(const Vector3& rhs) noexcept = default;
	constexpr Vector3& operator=(Vector3&& rhs) noexcept = default;
	constexpr Vector3& operator+=(const Vector3& rhs) noexcept { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	constexpr Vector3& operator-=(const Vector3& rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	constexpr Vector3& operator*=(const Vector3& rhs) noexcept { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
	constexpr Vector3& operator*=(const _Ty rhs) noexcept { x *= rhs; y *= rhs; z *= rhs; return *this; }
	constexpr Vector3& operator/=(const Vector3& rhs) noexcept { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }
	constexpr Vector3& operator/=(const _Ty rhs) noexcept { x /= rhs; y /= rhs; z /= rhs; return *this; }

	//* cast
	template <class _U>
	operator Vector3<_U>() const noexcept { return { static_cast<_U>(x), static_cast<_U>(y), static_cast<_U>(z) }; }

	//* unary
	constexpr Vector3 operator+() const noexcept { return *this; }
	constexpr Vector3 operator-() const noexcept { return { -x, -y, -z }; }

	//* access
	constexpr _Ty& operator[](size_t index) noexcept { return (&x)[index]; }
	constexpr const _Ty& operator[](size_t index) const noexcept { return (&x)[index]; }

	//=========================================================================================
	// variables
	//=========================================================================================

	_Ty x, y, z;

	//=========================================================================================
	// formatter
	//=========================================================================================

	template <typename FormatContext>
	auto Format(FormatContext& ctx) const {
		return std::format_to(ctx.out(), "({}, {}, {})", x, y, z);
	}

	//=========================================================================================
	// mathematical methods
	//=========================================================================================

	//* member methods

	Vector3 constexpr Saturate() const {
		return { std::clamp(x, (_Ty)0, (_Ty)1), std::clamp(y, (_Ty)0, (_Ty)1), std::clamp(z, (_Ty)0, (_Ty)1) };
	}

	Vector3 constexpr Abs() const {
		return { std::abs(x), std::abs(y), std::abs(z) };
	}

	_Ty constexpr Length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	Vector3 constexpr Normalize() const {
		return Length() != 0 ? *this / Length() : Vector3();
	}

	Vector3 constexpr Floor() const {
		return { std::floor(x), std::floor(y), std::floor(z) };
	}

	//* static member methods

	static constexpr _Ty Dot(const Vector3& lhs, const Vector3& rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	static constexpr _Ty Distance(const Vector3& lhs, const Vector3& rhs) {
		return (lhs - rhs).Length();
	}

	static constexpr Vector3 Cross(const Vector3& lhs, const Vector3& rhs) {
		return {
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		};
	}

	static constexpr Vector3 Lerp(const Vector3& x, const Vector3& y, _Ty t) {
		return x + (y - x) * t;
	}

	static constexpr Vector3 Reflect(const Vector3& v, const Vector3& n) {
		return v - n * (2.0f * Dot(v, n));
	}

	static constexpr Vector3 Clamp(const Vector3& v, const Vector3& min, const Vector3& max) {
		return {
			std::clamp(v.x, min.x, max.x),
			std::clamp(v.y, min.y, max.y),
			std::clamp(v.z, min.z, max.z)
		};
	}

	static constexpr Vector3 Min(const Vector3& lhs, const Vector3& rhs) {
		return {
			std::min(lhs.x, rhs.x),
			std::min(lhs.y, rhs.y),
			std::min(lhs.z, rhs.z)
		};
	}

	static constexpr Vector3 Max(const Vector3& lhs, const Vector3& rhs) {
		return {
			std::max(lhs.x, rhs.x),
			std::max(lhs.y, rhs.y),
			std::max(lhs.z, rhs.z)
		};
	}

	static constexpr Vector3 Floor(const Vector3& v) {
		return {
			std::floor(v.x),
			std::floor(v.y),
			std::floor(v.z)
		};
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Vector3 class binary operators
////////////////////////////////////////////////////////////////////////////////////////////

template <class _Ty>
constexpr Vector3<_Ty> operator+(const Vector3<_Ty>& lhs, const Vector3<_Ty>& rhs) noexcept {
	return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}

template <class _Ty>
constexpr Vector3<_Ty> operator-(const Vector3<_Ty>& lhs, const Vector3<_Ty>& rhs) noexcept {
	return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}

template <class _Ty>
constexpr Vector3<_Ty> operator*(const Vector3<_Ty>& lhs, const Vector3<_Ty>& rhs) noexcept {
	return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
}

template <class _Ty>
constexpr Vector3<_Ty> operator*(const Vector3<_Ty>& lhs, const _Ty rhs) noexcept {
	return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
}

template <class _Ty>
constexpr Vector3<_Ty> operator*(const _Ty lhs, const Vector3<_Ty>& rhs) noexcept {
	return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z };
}

template <class _Ty>
constexpr Vector3<_Ty> operator/(const Vector3<_Ty>& lhs, const Vector3<_Ty>& rhs) noexcept {
	return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
}

template <class _Ty>
constexpr Vector3<_Ty> operator/(const Vector3<_Ty>& lhs, const _Ty rhs) noexcept {
	return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs };
}

////////////////////////////////////////////////////////////////////////////////////////////
// Vector3 formatter structure
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
struct std::formatter<Vector3<T>> : Sxl::BaseFormatter<Vector3<T>> {};

////////////////////////////////////////////////////////////////////////////////////////////
// constexpr constants
////////////////////////////////////////////////////////////////////////////////////////////

template <class _Ty = float>
constexpr Vector3<_Ty> kOrigin3 = { 0, 0, 0 };

template <class _Ty = float>
constexpr Vector3<_Ty> kUnit3 = { 1, 1, 1 };

template <class _Ty = float>
constexpr Vector3<_Ty> kUnitX3 = { 1, 0, 0 };

template <class _Ty = float>
constexpr Vector3<_Ty> kUnitY3 = { 0, 1, 0 };

template <class _Ty = float>
constexpr Vector3<_Ty> kUnitZ3 = { 0, 0, 1 };

template <class _Ty = float>
constexpr Vector3<_Ty> kInfinity3 = { std::numeric_limits<_Ty>::infinity(), std::numeric_limits<_Ty>::infinity(), std::numeric_limits<_Ty>::infinity() };

//* directions
template <class _Ty = float>
constexpr Vector3<_Ty> kForward3 = kUnitZ3<_Ty>;

template <class _Ty = float>
constexpr Vector3<_Ty> kBackward3 = -kUnitZ3<_Ty>;

template <class _Ty = float>
constexpr Vector3<_Ty> kUp3 = kUnitY3<_Ty>;

template <class _Ty = float>
constexpr Vector3<_Ty> kDown3 = -kUnitY3<_Ty>;

template <class _Ty = float>
constexpr Vector3<_Ty> kRight3 = kUnitX3<_Ty>;

template <class _Ty = float>
constexpr Vector3<_Ty> kLeft3 = -kUnitX3<_Ty>;

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

using Vector3i  = Vector3<std::int32_t>;
using Vector3ui = Vector3<std::uint32_t>;
using Vector3uz = Vector3<std::size_t>;

using Vector3f = Vector3<float>;


