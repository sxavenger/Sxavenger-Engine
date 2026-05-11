#include "TransformationMatrix.h"

////////////////////////////////////////////////////////////////////////////////////////////
// TransformationMatrix structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void TransformationMatrix::Transfer(const Transformation& t) {
	mat                 = t.ToMatrix();
	matInverseTranspose = t.Inverse().ToMatrix().Transpose();
}

void TransformationMatrix::Transfer(const Matrix4x4& _mat) {
	mat                 = _mat;
	matInverseTranspose = _mat.Inverse().Transpose();
}

TransformationMatrix TransformationMatrix::Identity() noexcept {
	TransformationMatrix v = {};
	v.mat                 = Matrix4x4::Identity();
	v.matInverseTranspose = Matrix4x4::Identity();

	return v;
}
