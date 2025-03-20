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
	return Matrix4x4::MakeAffine(scale, rotate, translate);
}

////////////////////////////////////////////////////////////////////////////////////////////
// QuaternionTransform structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void QuaternionTransform::SetImGuiCommand(float granularityTranslate, float granularityRotate, float granularityScale) {
	ImGui::DragFloat3("translate", &translate.x, granularityTranslate);

	Vector3f drag = {};
	if (ImGui::DragFloat3("## rotation drager", &drag.x, granularityRotate, -1.0f, 1.0f, "", ImGuiSliderFlags_NoInput)) {
		rotate *= Quaternion::AxisAngle({ 1.0f, 0.0f, 0.0f }, drag.x);
		rotate *= Quaternion::AxisAngle({ 0.0f, 1.0f, 0.0f }, drag.y);
		rotate *= Quaternion::AxisAngle({ 0.0f, 0.0f, 1.0f }, drag.z);
	}

	ImGui::SameLine();

	if (ImGui::Button("reset")) {
		rotate = Quaternion::Identity();
	}

	ImGui::SameLine();
	ImGui::Text("rotation");


	ImGui::DragFloat3("scale", &scale.x, granularityScale);
}

Matrix4x4 QuaternionTransform::ToMatrix() const {
	return Matrix4x4::MakeAffine(scale, rotate, translate);
}

json QuaternionTransform::OutputJson() const {
	json data;
	data["translate"] = GeometryJsonSerializer::ToJson(translate);
	data["rotate"]    = GeometryJsonSerializer::ToJson(rotate);
	data["scale"]     = GeometryJsonSerializer::ToJson(scale);
	return data;
}

void QuaternionTransform::InputJson(const json& data) {
	translate = GeometryJsonSerializer::JsonToVector3f(data.at("translate"));
	rotate    = GeometryJsonSerializer::JsonToQuaternion(data.at("rotate"));
	scale     = GeometryJsonSerializer::JsonToVector3f(data.at("scale"));
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

void Transform2d::SetImGuiCommand(float granularityTranslate, float granularityScale) {
	ImGui::DragFloat2("translate", &translate.x, granularityTranslate);
	ImGui::SliderAngle("rotate",   &rotate);
	ImGui::DragFloat2("scale",     &scale.x, granularityScale);
}

Matrix4x4 Transform2d::ToMatrix() const {
	return Matrix4x4::MakeAffine({ scale.x, scale.y, 0.0f }, { 0.0f, 0.0f, rotate }, { translate.x, translate.y, 0.0f });
}

json Transform2d::OutputJson() const {
	json data;
	data["translate"] = GeometryJsonSerializer::ToJson(translate);
	data["rotate"]    = rotate;
	data["scale"]     = GeometryJsonSerializer::ToJson(scale);
	return data;
}

void Transform2d::InputJson(const json& data) {
	translate = GeometryJsonSerializer::JsonToVector2f(data.at("translate"));
	rotate    = data.at("rotate");
	scale     = GeometryJsonSerializer::JsonToVector2f(data.at("scale"));
}
