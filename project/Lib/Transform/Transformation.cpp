#include "Transformation.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Transformation structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Transformation Transformation::MakeAffine(const Vector3f& scale, const Vector3f& rotate, const Vector3f& translate) noexcept {
	Transformation v = {};
	v.basis = Basis::MakeScale(scale) * Basis::MakeRotateEuler(rotate);
	v.translate = translate;

	return v;
}

Transformation Transformation::MakeAffine(const Vector3f& scale, const Quaternion& rotate, const Vector3f& translate) noexcept {
	Transformation v = {};
	v.basis = Basis::MakeScale(scale) * Basis::MakeRotateQuaternion(rotate);
	v.translate = translate;

	return v;
}
