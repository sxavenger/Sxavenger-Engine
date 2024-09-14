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

/* Any */

constexpr inline bool Any(const Flag4& f) {
	return f.x || f.y || f.z || f.w;
}

constexpr inline bool Any(const Flag3& f) {
	return f.x || f.y || f.z;
}