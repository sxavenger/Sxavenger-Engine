#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <numbers>
#include <limits>

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
