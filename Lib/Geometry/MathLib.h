#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// vector
#include <Vector4.h>
#include <Vector3.h>
#include <Vector2.h>

// matrix
#include <Matrix4x4.h>

// c++
#include <cmath>
#include <numbers>

////////////////////////////////////////////////////////////////////////////////////////////
// MathLib methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f Normalize(const Vector3f& v);

float Length(const Vector3f& v);

float Dot(const Vector3f& x, const Vector3f& y);

Vector3f Cross(const Vector3f& x, const Vector3f& y);

Vector3f Reflect(const Vector3f& i, const Vector3f& n);

Vector3f Lerp(const Vector3f& x, const Vector3f& y, float s);


////////////////////////////////////////////////////////////////////////////////////////////
// constexpr
////////////////////////////////////////////////////////////////////////////////////////////

constexpr const float pi_v = std::numbers::pi_v<float>;