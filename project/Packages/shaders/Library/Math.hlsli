#pragma once

#ifndef _MATH_LIBRAY
#define _MATH_LIBRAY

////////////////////////////////////////////////////////////////////////////////////////////
// constants
////////////////////////////////////////////////////////////////////////////////////////////

static const float kPi  = 3.141592653589793f;
static const float kTau = kPi * 2.0f;

static const float kPiInv  = 1.0f / kPi;
static const float kTauInv = 1.0f / kTau;

static const float kEpsilon = 1.192092896e-07f;

float Square(float value) { return value * value; }

#endif
