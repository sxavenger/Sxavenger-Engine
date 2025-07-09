#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* geometry
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"

//* c++
#include <bitset>

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

using Flag2 = Vector2<bool>; //!< Vector2 flag
using Flag3 = Vector3<bool>; //!< Vector3 flag
using Flag4 = Vector4<bool>; //!< Vector4 flag

////////////////////////////////////////////////////////////////////////////////////////////
// comparision operators
////////////////////////////////////////////////////////////////////////////////////////////

//* ==

template <class _Ty>
constexpr Flag2 operator==(const Vector2<_Ty>& lhs, const Vector2<_Ty>& rhs) noexcept {
	return { lhs.x == rhs.x, lhs.y == rhs.y };
}

template <class _Ty>
constexpr Flag3 operator==(const Vector3<_Ty>& lhs, const Vector3<_Ty>& rhs) noexcept {
	return { lhs.x == rhs.x, lhs.y == rhs.y, lhs.z == rhs.z };
}

template <class _Ty>
constexpr Flag4 operator==(const Vector4<_Ty>& lhs, const Vector4<_Ty>& rhs) noexcept {
	return { lhs.x == rhs.x, lhs.y == rhs.y, lhs.z == rhs.z, lhs.w == rhs.w };
}

//* !=

template <class _Ty>
constexpr Flag2 operator!=(const Vector2<_Ty>& lhs, const Vector2<_Ty>& rhs) noexcept {
	return { lhs.x != rhs.x, lhs.y != rhs.y };
}

template <class _Ty>
constexpr Flag3 operator!=(const Vector3<_Ty>& lhs, const Vector3<_Ty>& rhs) noexcept {
	return { lhs.x != rhs.x, lhs.y != rhs.y, lhs.z != rhs.z };
}

template <class _Ty>
constexpr Flag4 operator!=(const Vector4<_Ty>& lhs, const Vector4<_Ty>& rhs) noexcept {
	return { lhs.x != rhs.x, lhs.y != rhs.y, lhs.z != rhs.z, lhs.w != rhs.w };
}

//* <

template <class _Ty>
constexpr Flag2 operator<(const Vector2<_Ty>& lhs, const Vector2<_Ty>& rhs) noexcept {
	return { lhs.x < rhs.x, lhs.y < rhs.y };
}

template <class _Ty>
constexpr Flag3 operator<(const Vector3<_Ty>& lhs, const Vector3<_Ty>& rhs) noexcept {
	return { lhs.x < rhs.x, lhs.y < rhs.y, lhs.z < rhs.z };
}

template <class _Ty>
constexpr Flag4 operator<(const Vector4<_Ty>& lhs, const Vector4<_Ty>& rhs) noexcept {
	return { lhs.x < rhs.x, lhs.y < rhs.y, lhs.z < rhs.z, lhs.w < rhs.w };
}

//* >

template <class _Ty>
constexpr Flag2 operator>(const Vector2<_Ty>& lhs, const Vector2<_Ty>& rhs) noexcept {
	return { lhs.x > rhs.x, lhs.y > rhs.y };
}

template <class _Ty>
constexpr Flag3 operator>(const Vector3<_Ty>& lhs, const Vector3<_Ty>& rhs) noexcept {
	return { lhs.x > rhs.x, lhs.y > rhs.y, lhs.z > rhs.z };
}

template <class _Ty>
constexpr Flag4 operator>(const Vector4<_Ty>& lhs, const Vector4<_Ty>& rhs) noexcept {
	return { lhs.x > rhs.x, lhs.y > rhs.y, lhs.z > rhs.z, lhs.w > rhs.w };
}

//* <=

template <class _Ty>
constexpr Flag2 operator<=(const Vector2<_Ty>& lhs, const Vector2<_Ty>& rhs) noexcept {
	return { lhs.x <= rhs.x, lhs.y <= rhs.y };
}

template <class _Ty>
constexpr Flag3 operator<=(const Vector3<_Ty>& lhs, const Vector3<_Ty>& rhs) noexcept {
	return { lhs.x <= rhs.x, lhs.y <= rhs.y, lhs.z <= rhs.z };
}

template <class _Ty>
constexpr Flag4 operator<=(const Vector4<_Ty>& lhs, const Vector4<_Ty>& rhs) noexcept {
	return { lhs.x <= rhs.x, lhs.y <= rhs.y, lhs.z <= rhs.z, lhs.w <= rhs.w };
}

//* >=

template <class _Ty>
constexpr Flag2 operator>=(const Vector2<_Ty>& lhs, const Vector2<_Ty>& rhs) noexcept {
	return { lhs.x >= rhs.x, lhs.y >= rhs.y };
}

template <class _Ty>
constexpr Flag3 operator>=(const Vector3<_Ty>& lhs, const Vector3<_Ty>& rhs) noexcept {
	return { lhs.x >= rhs.x, lhs.y >= rhs.y, lhs.z >= rhs.z };
}

template <class _Ty>
constexpr Flag4 operator>=(const Vector4<_Ty>& lhs, const Vector4<_Ty>& rhs) noexcept {
	return { lhs.x >= rhs.x, lhs.y >= rhs.y, lhs.z >= rhs.z, lhs.w >= rhs.w };
}

////////////////////////////////////////////////////////////////////////////////////////////
// comparision methods
////////////////////////////////////////////////////////////////////////////////////////////

//* all

constexpr bool All(const Flag2& flag) noexcept {
	return flag.x && flag.y;
}

constexpr bool All(const Flag3& flag) noexcept {
	return flag.x && flag.y && flag.z;
}

constexpr bool All(const Flag4& flag) noexcept {
	return flag.x && flag.y && flag.z && flag.w;
}

//* any

constexpr bool Any(const Flag2& flag) noexcept {
	return flag.x || flag.y;
}

constexpr bool Any(const Flag3& flag) noexcept {
	return flag.x || flag.y || flag.z;
}

constexpr bool Any(const Flag4& flag) noexcept {
	return flag.x || flag.y || flag.z || flag.w;
}
