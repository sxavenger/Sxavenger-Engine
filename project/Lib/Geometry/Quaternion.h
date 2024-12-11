#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* geometry
#include "Vector3.h"

//* lib
#include <Lib/Sxl/Formatter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion class
////////////////////////////////////////////////////////////////////////////////////////////
class Quaternion {
public:

	//=========================================================================================
	// constructor
	//=========================================================================================

	Quaternion() = default;
	Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

	//=========================================================================================
	// methods
	//=========================================================================================

	static Quaternion Identity();

	Quaternion Conjugation() const;

	float Norm() const;

	Quaternion Normalize() const;

	Quaternion Inverse() const;

	//=========================================================================================
	// compound assignment operator
	//=========================================================================================

	Quaternion& operator*=(const Quaternion& q);

	//=========================================================================================
	// variables
	//=========================================================================================

	float x, y, z, w;

	//=========================================================================================
	// formatter
	//=========================================================================================

	template <typename FormatContext>
	auto format(FormatContext& ctx) const {
		return std::format_to(ctx.out(), "(x: {}, y: {}, z: {}, w: {})", x, y, z, w);
	}

};

//=========================================================================================
// binary operator
//=========================================================================================

Quaternion operator+(const Quaternion& q, const Quaternion& r);

Quaternion operator*(const Quaternion& q, const Quaternion& r);

Quaternion operator*(const Quaternion& q, float scaler);
Quaternion operator*(float scaler, const Quaternion& q);

//=========================================================================================
// unary operator
//=========================================================================================

inline Quaternion operator+(const Quaternion& q) {
	return q;
}

inline Quaternion operator-(const Quaternion& q) {
	return { -q.x, -q.y, -q.z, -q.w };
}

////////////////////////////////////////////////////////////////////////////////////////////
// formatter
////////////////////////////////////////////////////////////////////////////////////////////
template <>
struct std::formatter<Quaternion> : Sxl::BaseFormatter<Quaternion> {};

////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion methods
////////////////////////////////////////////////////////////////////////////////////////////

float Dot(const Quaternion& q, const Quaternion& r);

Quaternion MakeAxisAngle(const Vector3f& axis, float angle);

Vector3f RotateVector(const Vector3f& v, const Quaternion& q);

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

Quaternion ToQuaternion(const Vector3f& euler);
Quaternion ToQuaternion2(const Vector3f& euler);

Quaternion LookAt(const Vector3f& u, const Vector3f& v);