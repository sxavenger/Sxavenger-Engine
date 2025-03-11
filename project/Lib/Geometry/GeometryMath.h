#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cmath>
#include <numbers>

////////////////////////////////////////////////////////////////////////////////////////////
// forawrd
////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class Vector2;

template <class T>
class Vector3;

template <class T>
class Vector4;

class Quaternion;

class Matrix4x4;

////////////////////////////////////////////////////////////////////////////////////////////
// vector methods
////////////////////////////////////////////////////////////////////////////////////////////

float Length(const Vector2<float>& v);
float Length(const Vector3<float>& v);
float Length(const Vector4<float>& v);

float Dot(const Vector2<float>& x, const Vector2<float>& y);
float Dot(const Vector3<float>& x, const Vector3<float>& y);
float Dot(const Vector4<float>& x, const Vector4<float>& y);

float Cross(const Vector2<float>& x, const Vector2<float>& y);
Vector3<float> Cross(const Vector3<float>& x, const Vector3<float>& y);

Vector2<float> Normalize(const Vector2<float>& v);
Vector3<float> Normalize(const Vector3<float>& v);
Vector4<float> Normalize(const Vector4<float>& v);

Vector2<float> Saturate(const Vector2<float>& v);
Vector3<float> Saturate(const Vector3<float>& v);
Vector4<float> Saturate(const Vector4<float>& v);

Vector2<float> Abs(const Vector2<float>& v);
Vector3<float> Abs(const Vector3<float>& v);
Vector4<float> Abs(const Vector4<float>& v);

Vector2<float> Lerp(const Vector2<float>& x, const Vector2<float>& y, float s);
Vector3<float> Lerp(const Vector3<float>& x, const Vector3<float>& y, float s);
Vector4<float> Lerp(const Vector4<float>& x, const Vector4<float>& y, float s);

Vector2<float> Clamp(const Vector2<float>& v, const Vector2<float>& min, const Vector2<float>& max);
Vector3<float> Clamp(const Vector3<float>& v, const Vector3<float>& min, const Vector3<float>& max);
Vector4<float> Clamp(const Vector4<float>& v, const Vector4<float>& min, const Vector4<float>& max);

////////////////////////////////////////////////////////////////////////////////////////////
// quaterion methods
////////////////////////////////////////////////////////////////////////////////////////////

Quaternion AxisAngle(const Vector3<float>& axis, float angle);
Quaternion MakeAxisAngle(const Vector3<float>& axis, float angle); //!< 削除予定

Quaternion Slerp(const Quaternion& x, const Quaternion& y, float t);

float Dot(const Quaternion& x, const Quaternion& y);

Quaternion ToQuaternion(const Vector3<float>& euler);

Quaternion FromToRotation(const Vector3<float>& from, const Vector3<float>& to);

Vector3<float> RotateVector(const Vector3<float>& v, const Quaternion& q);

////////////////////////////////////////////////////////////////////////////////////////////
// matrix methods
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
// vector math methods
////////////////////////////////////////////////////////////////////////////////////////////

bool IsNan(const Vector2<float>& v);
bool IsNan(const Vector3<float>& v);
bool IsNan(const Vector4<float>& v);
bool IsNan(const Quaternion& q);

////////////////////////////////////////////////////////////////////////////////////////////
// angle methods
////////////////////////////////////////////////////////////////////////////////////////////

constexpr const float pi_v  = std::numbers::pi_v<float>;
constexpr const float tau_v = std::numbers::pi_v<float> * 2.0f;

constexpr const float kRadToDeg = 180.0f / pi_v;
constexpr const float kDegToRad = pi_v / 180.0f;
