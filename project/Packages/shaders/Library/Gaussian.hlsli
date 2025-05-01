#pragma once

/*
#reference
 - https://en.wikipedia.org/wiki/Normal_distribution
 - https://www.fbs.osaka-u.ac.jp/labs/ishijima/2D-Gaussian-11.html
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Math.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float Gaussian(float x, float sigma) {
	float a = 1.0f / sqrt(2.0f * kPi * sigma * sigma);
	float b = exp(-(x * x) / (2.0f * sigma * sigma));
	return a * b;
}

float Gaussian2d(float2 x, float sigma) {
	float a = 1.0f / (2.0f * kPi * sigma * sigma);
	float b = exp(-(x.x * x.x + x.y * x.y) / (2.0f * sigma * sigma));
	return a * b;
}
