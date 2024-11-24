#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// vector
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4x4.h"
#include "VectorComparison.h"

// c++
#include <cmath>
#include <numbers>

////////////////////////////////////////////////////////////////////////////////////////////
// MathLib methods
////////////////////////////////////////////////////////////////////////////////////////////

bool IsNan(const Vector3f& v);
bool IsNan(const Quaternion& q);

Vector3f Project(const Vector3f& v1, const Vector3f& v2);

////////////////////////////////////////////////////////////////////////////////////////////
// constexpr
////////////////////////////////////////////////////////////////////////////////////////////

constexpr const float pi_v = std::numbers::pi_v<float>;

constexpr const float kRadToDeg = 180.0f / pi_v;
constexpr const float kDegToRad = pi_v / 180.0f;