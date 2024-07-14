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

// com
#include <VectorComparison.h>

// c++
#include <cmath>
#include <numbers>

////////////////////////////////////////////////////////////////////////////////////////////
// MathLib methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector4f Lerp(const Vector4f& x, const Vector4f& y, float s);

bool IsNan(const Vector3f& v);
bool IsNan(const Quaternion& q);


////////////////////////////////////////////////////////////////////////////////////////////
// constexpr
////////////////////////////////////////////////////////////////////////////////////////////

constexpr const float pi_v = std::numbers::pi_v<float>;