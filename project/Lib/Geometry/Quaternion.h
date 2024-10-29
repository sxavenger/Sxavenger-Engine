#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Vector3.h"

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
// Quaternion methods
////////////////////////////////////////////////////////////////////////////////////////////

float Dot(const Quaternion& q, const Quaternion& r);

Quaternion MakeAxisAngle(const Vector3f& axis, float angle);

Vector3f RotateVector(const Vector3f& v, const Quaternion& q);

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

Quaternion ToQuaternion(const Vector3f& euler);

Quaternion LookAt(const Vector3f& u, const Vector3f& v);

