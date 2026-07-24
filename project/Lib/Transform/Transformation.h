#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* transformation
#include "Basis.h"

//* math
#include "../Math/Matrix4x4.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Transformation structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Transformation {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	constexpr Transformation() noexcept = default;

	constexpr Transformation(const Transformation&) noexcept = default;
	constexpr Transformation(Transformation&&) noexcept      = default;

	constexpr Transformation(const Basis& _basis, const Vector3f& _translate) noexcept : basis(_basis), translate(_translate) {};
	constexpr Transformation(Basis&& _basis, Vector3f&& _translate) noexcept : basis(std::move(_basis)), translate(std::move(_translate)) {};

	//* operator *//

	constexpr Transformation& operator=(const Transformation&) noexcept = default;
	constexpr Transformation& operator=(Transformation&&) noexcept      = default;

	constexpr Transformation& operator*=(const Transformation& rhs) noexcept;

	//* mathmatical methods *//

	constexpr Transformation Inverse() const noexcept;

	constexpr Matrix4x4 ToMatrix() const noexcept;

	static constexpr Transformation Identity() noexcept;

	static Transformation MakeAffine(const Vector3f& scale, const Vector3f& rotate, const Vector3f& translate) noexcept;
	static Transformation MakeAffine(const Vector3f& scale, const Quaternion& rotate, const Vector3f& translate) noexcept;

	constexpr static Vector3f Transform(const Vector3f& v, const Transformation& t) noexcept;

	//=========================================================================================
	// public variables
	//=========================================================================================

	Basis basis        = {};
	Vector3f translate = {};

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// Transformation structure constexpr methods
////////////////////////////////////////////////////////////////////////////////////////////

inline constexpr Transformation& Transformation::operator*=(const Transformation& rhs) noexcept {

	Transformation v = {};
	v.basis     = basis * rhs.basis;
	v.translate = Basis::TransformNormal(translate, rhs.basis) + rhs.translate;

	*this = v;
	return *this;

}

inline constexpr Transformation Transformation::Inverse() const noexcept {
	Transformation v = {};
	v.basis     = basis.Inverse();
	v.translate = Basis::TransformNormal(-translate, v.basis);
	return v;
}

inline constexpr Matrix4x4 Transformation::ToMatrix() const noexcept {
	return Matrix4x4{
		basis.m[0][0], basis.m[0][1], basis.m[0][2], 0.0f,
		basis.m[1][0], basis.m[1][1], basis.m[1][2], 0.0f,
		basis.m[2][0], basis.m[2][1], basis.m[2][2], 0.0f,
		translate.x,   translate.y,   translate.z,   1.0f
	};
}

inline constexpr Transformation Transformation::Identity() noexcept {
	return { Basis::Identity(), Vector3f{} };
}

inline constexpr Vector3f Transformation::Transform(const Vector3f& v, const Transformation& t) noexcept {
	return Basis::TransformNormal(v, t.basis) + t.translate;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Basis class binary operators
////////////////////////////////////////////////////////////////////////////////////////////

constexpr Transformation operator*(const Transformation& lhs, const Transformation& rhs) noexcept {
	Transformation v = {};
	v.basis     = lhs.basis * rhs.basis;
	v.translate = Basis::TransformNormal(lhs.translate, rhs.basis) + rhs.translate;
	return v;
}
