#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* geometry
#include "Vector3.h"

//* sxl
#include <Lib/Sxl/Formatter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion class
////////////////////////////////////////////////////////////////////////////////////////////
class Quaternion {
public:

	//=========================================================================================
	// constructor
	//=========================================================================================

	constexpr Quaternion() noexcept = default;
	constexpr Quaternion(float _x, float _y, float _z, float _w) noexcept : imaginary(_x, _y, _z), w(_w) {};
	constexpr Quaternion(const Quaternion& rhs) noexcept = default;
	constexpr Quaternion(Quaternion&& rhs) noexcept = default;
	constexpr Quaternion(const Vector3f& _imaginary, float _w) noexcept : imaginary(_imaginary), w(_w) {};

	//=========================================================================================
	// operator
	//=========================================================================================

	//* compound assignment
	constexpr Quaternion& operator=(const Quaternion& rhs) noexcept = default;
	constexpr Quaternion& operator=(Quaternion&& rhs) noexcept = default;
	constexpr Quaternion& operator+=(const Quaternion& rhs) noexcept;
	constexpr Quaternion& operator-=(const Quaternion& rhs) noexcept;
	constexpr Quaternion& operator*=(const Quaternion& rhs) noexcept;

	//* unary
	constexpr Quaternion operator+() const noexcept;
	constexpr Quaternion operator-() const noexcept;

	//=========================================================================================
	// public variables
	//=========================================================================================

	Vector3f imaginary; //!< 虚部ベクトル
	float w;            //!< 実部

	//=========================================================================================
	// mathmatical methods
	//=========================================================================================

	Quaternion Conjugate() const noexcept;

	Quaternion Inverse() const noexcept;

	Quaternion Normalize() const noexcept;

	float Norm() const noexcept;

	static Quaternion Identity() noexcept;

	static Quaternion AxisAngle(const Vector3f& axis, float angle) noexcept;

	static Vector3f RotateVector(const Vector3f& v, const Quaternion& q) noexcept;

	static Quaternion ToQuaternion(const Vector3f& euler) noexcept;

	static float Dot(const Quaternion& x, const Quaternion& y) noexcept;

	static Quaternion Slerp(const Quaternion& x, const Quaternion& y, float t) noexcept;

	static Quaternion FromToRotation(const Vector3f& from, const Vector3f& to) noexcept;

	static Quaternion LookForward(const Vector3f& forward, const Vector3f& up = kUnitY3<float>) noexcept;

	static Vector3f ToEuler(const Quaternion& q) noexcept; //!< test

	//=========================================================================================
	// formatter
	//=========================================================================================

	template <typename FormatContext>
	auto format(FormatContext& ctx) const {
		return std::format_to(ctx.out(), "({}, {}, {}, {})", imaginary.x, imaginary.y, imaginary.z, w);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion class constexpr methods
////////////////////////////////////////////////////////////////////////////////////////////

constexpr Quaternion& Quaternion::operator+=(const Quaternion& rhs) noexcept {
	imaginary += rhs.imaginary;
	w         += rhs.w;
	return *this;
}

constexpr Quaternion& Quaternion::operator-=(const Quaternion& rhs) noexcept {
	imaginary -= rhs.imaginary;
	w         -= rhs.w;
	return *this;
}

constexpr Quaternion& Quaternion::operator*=(const Quaternion& rhs) noexcept {
	Vector3f cross = Vector3f::Cross(imaginary, rhs.imaginary);
	float dot      = Vector3f::Dot(imaginary, rhs.imaginary);

	imaginary = cross + rhs.imaginary * w + imaginary * rhs.w;
	w         = w * rhs.w - dot;

	return *this;
}

constexpr Quaternion Quaternion::operator+() const noexcept {
	return *this;
}

constexpr Quaternion Quaternion::operator-() const noexcept {
	return { -imaginary, -w };
}

////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion formatter structure
////////////////////////////////////////////////////////////////////////////////////////////
template <>
struct std::formatter<Quaternion> : Sxl::BaseFormatter<Quaternion> {};

////////////////////////////////////////////////////////////////////////////////////////////
// Quaternion class binary operators
////////////////////////////////////////////////////////////////////////////////////////////

Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) noexcept;
Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs) noexcept;
Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs) noexcept;
Quaternion operator*(const Quaternion& lhs, float rhs) noexcept;
Quaternion operator*(float lhs, const Quaternion& rhs) noexcept;
