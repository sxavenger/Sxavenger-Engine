#include "Transform.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EulerTransform structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void EulerTransform::SetImGuiCommand(float granularityTranslate, float granularityRotate, float granularityScale) {
	ImGui::DragFloat3("translate", &translate.x, granularityTranslate);

	Vector3f deg = rotate * kRadToDeg;
	if (ImGui::DragFloat3("rotate", &deg.x, granularityRotate, 0.0f, 0.0f, "%.0f deg")) {
		rotate = deg * kDegToRad;
	}

	ImGui::DragFloat3("scale", &scale.x, granularityScale);
}

Matrix4x4 EulerTransform::ToMatrix() const {
	return Matrix::MakeAffine(scale, rotate, translate);
}

////////////////////////////////////////////////////////////////////////////////////////////
// QuaternionTransform structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void QuaternionTransform::SetImGuiCommand(float granularityTranslate, float granularityRotate, float granularityScale) {
	ImGui::DragFloat3("translate", &translate.x, granularityTranslate);

	Vector3f drag = {};
	if (ImGui::DragFloat3("rotation drager", &drag.x, granularityRotate, -1.0f, 1.0f, "", ImGuiSliderFlags_NoInput)) {
		rotate *= ToQuaternion(drag);
	}

	ImGui::DragFloat3("scale", &scale.x, granularityScale);
}

Matrix4x4 QuaternionTransform::ToMatrix() const {
	return Matrix::MakeAffine(scale, rotate, translate);
}

////////////////////////////////////////////////////////////////////////////////////////////
// TransformationMatrix structure
////////////////////////////////////////////////////////////////////////////////////////////

void TransformationMatrix::Init() {
	mat                 = Matrix4x4::Identity();
	matInverseTranspose = Matrix4x4::Identity();
}

void TransformationMatrix::Transfer(const Matrix4x4& _mat) {
	mat                 = _mat;
	matInverseTranspose = _mat.Inverse().Transpose();
}