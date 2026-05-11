#include "TransformEuler.h"

////////////////////////////////////////////////////////////////////////////////////////////
// TransformEuler structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Transformation TransformEuler::ToTransformation() const {
	return Transformation::MakeAffine(scale, rotate, translate);
}

Matrix4x4 TransformEuler::ToMatrix() const {
	return Matrix4x4::MakeAffine(scale, rotate, translate);
}
