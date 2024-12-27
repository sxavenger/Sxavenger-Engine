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
		rotate *= MakeAxisAngle({ 1.0f, 0.0f, 0.0f }, drag.x);
		rotate *= MakeAxisAngle({ 0.0f, 1.0f, 0.0f }, drag.y);
		rotate *= MakeAxisAngle({ 0.0f, 0.0f, 1.0f }, drag.z);
	}

	ImGui::DragFloat3("scale", &scale.x, granularityScale);
}

Matrix4x4 QuaternionTransform::ToMatrix() const {
	return Matrix::MakeAffine(scale, rotate, translate);
}

////////////////////////////////////////////////////////////////////////////////////////////
// TransformationMatrix structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void TransformationMatrix::Init() {
	mat                 = Matrix4x4::Identity();
	matInverseTranspose = Matrix4x4::Identity();
}

void TransformationMatrix::Transfer(const Matrix4x4& _mat) {
	mat                 = _mat;
	matInverseTranspose = _mat.Inverse().Transpose();
}

////////////////////////////////////////////////////////////////////////////////////////////
// UVTransform structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void UVTransform::SetImGuiCommand(float granularityTranslate, float granularityScale) {
	ImGui::DragFloat2("translate", &translate.x, granularityTranslate);
	ImGui::SliderAngle("rotate", &rotate);
	ImGui::DragFloat2("scale", &scale.x, granularityScale);
}

Matrix4x4 UVTransform::ToMatrix() const {
	return Matrix::MakeAffine({ scale.x, scale.y, 0.0f }, { 0.0f, 0.0f, rotate }, { translate.x, translate.y, 0.0f });
}
