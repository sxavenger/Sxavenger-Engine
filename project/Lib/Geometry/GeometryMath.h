#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* geometry
#include "Vector2.h"
#include "Vector3.h"

//* c++
#include <numbers>
#include <limits>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////
// mathematical constants
////////////////////////////////////////////////////////////////////////////////////////////

//* 円周率 と関連する定数

constexpr float kPi  = std::numbers::pi_v<float>;
constexpr float kTau = std::numbers::pi_v<float> * 2.0f;

constexpr float kRadToDeg = 180.0f / kPi;
constexpr float kDegToRad = kPi / 180.0f;

//* 数値計算における許容誤差

constexpr float kEpsilon = std::numeric_limits<float>::epsilon();

////////////////////////////////////////////////////////////////////////////////////////////
// mathematical functions
////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
constexpr T Abs(T x) { return x < 0 ? -x : x; }

template <class T>
constexpr Vector2<T> Abs(const Vector2<T>& x) { return { Abs(x.x), Abs(x.y) }; }

template <class T>
constexpr Vector3<T> Abs(const Vector3<T>& x) { return { Abs(x.x), Abs(x.y), Abs(x.z) }; }

template <class T>
constexpr T Min(T a, T b) { return a < b ? a : b; }

template <class T>
constexpr Vector2<T> Min(const Vector2<T>& a, const Vector2<T>& b) { return { Min(a.x, b.x), Min(a.y, b.y) }; }

template <class T>
constexpr Vector3<T> Min(const Vector3<T>& a, const Vector3<T>& b) { return { Min(a.x, b.x), Min(a.y, b.y), Min(a.z, b.z) }; }

template <class T>
constexpr T Max(T a, T b) { return a > b ? a : b; }

template <class T>
constexpr Vector2<T> Max(const Vector2<T>& a, const Vector2<T>& b) { return { Max(a.x, b.x), Max(a.y, b.y) }; }

template <class T>
constexpr Vector3<T> Max(const Vector3<T>& a, const Vector3<T>& b) { return { Max(a.x, b.x), Max(a.y, b.y), Max(a.z, b.z) }; }

template <class T>
constexpr T Clamp(T x, T min, T max) { return x < min ? min : (x > max ? max : x); }

template <class T>
constexpr Vector2<T> Clamp(const Vector2<T>& x, const Vector2<T>& min, const Vector2<T>& max) { return { Clamp(x.x, min.x, max.x), Clamp(x.y, min.y, max.y) }; }

template <class T>
constexpr Vector3<T> Clamp(const Vector3<T>& x, const Vector3<T>& min, const Vector3<T>& max) { return { Clamp(x.x, min.x, max.x), Clamp(x.y, min.y, max.y), Clamp(x.z, min.z, max.z) }; }

template <class T>
constexpr T Saturate(T x) { return Clamp(x, static_cast<T>(0), static_cast<T>(1)); }

template <class T>
constexpr Vector2<T> Saturate(const Vector2<T>& x) { return Clamp(x, kOrigin2<T>, kUnit2<T>); }

template <class T>
constexpr Vector3<T> Saturate(const Vector3<T>& x) { return Clamp(x, kOrigin3<T>, kUnit3<T>); }
