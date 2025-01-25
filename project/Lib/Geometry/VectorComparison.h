#pragma once

//*****************************************************************************************
// Vectorどうしでの比較を行うoperatorと関数
//*****************************************************************************************

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

using Flag4 = Vector4<bool>;
using Flag3 = Vector3<bool>;
using Flag2 = Vector2<bool>;

////////////////////////////////////////////////////////////////////////////////////////////
// operator
////////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------
// a == b
//-----------------------------------------------------------------------------------------

template <typename T>
constexpr inline Flag4 operator==(const Vector4<T>& a, const Vector4<T>& b) {
	return { a.x == b.x, a.y == b.y, a.z == b.z, a.w == b.w };
}

template <typename T>
constexpr inline Flag3 operator==(const Vector3<T>& a, const Vector3<T>& b) {
	return { a.x == b.x, a.y == b.y, a.z == b.z };
}

template <typename T>
constexpr inline Flag2 operator==(const Vector2<T>& a, const Vector2<T>& b) {
	return { a.x == b.x, a.y == b.y };
}

//-----------------------------------------------------------------------------------------
// a != b
//-----------------------------------------------------------------------------------------

template <typename T>
constexpr inline Flag4 operator!=(const Vector4<T>& a, const Vector4<T>& b) {
	return { a.x != b.x, a.y != b.y, a.z != b.z, a.w != b.w };
}

template <typename T>
constexpr inline Flag3 operator!=(const Vector3<T>& a, const Vector3<T>& b) {
	return { a.x != b.x, a.y != b.y, a.z != b.z };
}

template <typename T>
constexpr inline Flag2 operator!=(const Vector2<T>& a, const Vector2<T>& b) {
	return { a.x != b.x, a.y != b.y };
}

//-----------------------------------------------------------------------------------------
// a >= b
//-----------------------------------------------------------------------------------------

template <typename T>
constexpr inline Flag4 operator>=(const Vector4<T>& a, const Vector4<T>& b) {
	return { a.x >= b.x, a.y >= b.y, a.z >= b.z, a.w >= b.w };
}

template <typename T>
constexpr inline Flag3 operator>=(const Vector3<T>& a, const Vector3<T>& b) {
	return { a.x >= b.x, a.y >= b.y, a.z >= b.z };
}

template <typename T>
constexpr inline Flag2 operator>=(const Vector2<T>& a, const Vector2<T>& b) {
	return { a.x >= b.x, a.y >= b.y };
}

//-----------------------------------------------------------------------------------------
// a > b
//-----------------------------------------------------------------------------------------

template <typename T>
constexpr inline Flag4 operator>(const Vector4<T>& a, const Vector4<T>& b) {
	return { a.x > b.x, a.y > b.y, a.z > b.z, a.w > b.w };
}

template <typename T>
constexpr inline Flag3 operator>(const Vector3<T>& a, const Vector3<T>& b) {
	return { a.x > b.x, a.y > b.y, a.z > b.z };
}

template <typename T>
constexpr inline Flag2 operator>(const Vector2<T>& a, const Vector2<T>& b) {
	return { a.x > b.x, a.y > b.y };
}

//-----------------------------------------------------------------------------------------
// a <= b
//-----------------------------------------------------------------------------------------

template <typename T>
constexpr inline Flag4 operator<=(const Vector4<T>& a, const Vector4<T>& b) {
	return { a.x <= b.x, a.y <= b.y, a.z <= b.z, a.w <= b.w };
}

template <typename T>
constexpr inline Flag3 operator<=(const Vector3<T>& a, const Vector3<T>& b) {
	return { a.x <= b.x, a.y <= b.y, a.z <= b.z };
}

template <typename T>
constexpr inline Flag2 operator<=(const Vector2<T>& a, const Vector2<T>& b) {
	return { a.x <= b.x, a.y <= b.y };
}

//-----------------------------------------------------------------------------------------
// a < b
//-----------------------------------------------------------------------------------------

template <typename T>
constexpr inline Flag4 operator<(const Vector4<T>& a, const Vector4<T>& b) {
	return { a.x < b.x, a.y < b.y, a.z < b.z, a.w < b.w };
}

template <typename T>
constexpr inline Flag3 operator<(const Vector3<T>& a, const Vector3<T>& b) {
	return { a.x < b.x, a.y < b.y, a.z < b.z };
}

template <typename T>
constexpr inline Flag2 operator<(const Vector2<T>& a, const Vector2<T>& b) {
	return { a.x < b.x, a.y < b.y };
}

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

/* All */

constexpr inline bool All(const Flag4& f) {
	return f.x && f.y && f.z && f.w;
}

constexpr inline bool All(const Flag3& f) {
	return f.x && f.y && f.z;
}

constexpr inline bool All(const Flag2& f) {
	return f.x && f.y;
}

/* Any */

constexpr inline bool Any(const Flag4& f) {
	return f.x || f.y || f.z || f.w;
}

constexpr inline bool Any(const Flag3& f) {
	return f.x || f.y || f.z;
}

constexpr inline bool Any(const Flag2& f) {
	return f.x || f.y;
}
