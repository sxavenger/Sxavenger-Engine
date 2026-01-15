#include "Transform.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

//* engine
#include <Engine/System/UI/SxImGui.h>

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
	SxImGui::DragVector3("translate", &translate.x, granularityTranslate);


	Vector3f e = Quaternion::ToEuler(rotate);
	if (SxImGui::DragVector3("rotate", &e.x, granularityRotate)) {
		rotate = Quaternion::ToQuaternion(e);
	}

	SxImGui::DragVector3("scale", &scale.x, granularityScale);
}

Matrix4x4 QuaternionTransform::ToMatrix() const {
	return Matrix4x4::MakeAffine(scale, rotate, translate);
}

Vector3f QuaternionTransform::GetForward() const {
	return Quaternion::RotateVector(kForward3<>, rotate);
}

json QuaternionTransform::ParseToJson() const {
	json data = json::object();
	data["translate"] = JsonSerializeFormatter<Vector3f>::Serialize(translate);
	data["rotate"]    = JsonSerializeFormatter<Quaternion>::Serialize(rotate);
	data["scale"]     = JsonSerializeFormatter<Vector3f>::Serialize(scale);
	return data;
}

void QuaternionTransform::InputJson(const json& data) {
	translate = JsonSerializeFormatter<Vector3f>::Deserialize(data.at("translate"));
	rotate    = JsonSerializeFormatter<Quaternion>::Deserialize(data.at("rotate"));
	scale     = JsonSerializeFormatter<Vector3f>::Deserialize(data.at("scale"));
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

json Transform2d::ParseToJson() const {
	json data = json::object();
	data["translate"] = JsonSerializeFormatter<Vector2f>::Serialize(translate);
	data["rotate"]    = JsonSerializeFormatter<float>::Serialize(rotate);
	data["scale"]     = JsonSerializeFormatter<Vector2f>::Serialize(scale);
	return data;
}

void Transform2d::InputJson(const json& data) {
	translate = JsonSerializeFormatter<Vector2f>::Deserialize(data.at("translate"));
	rotate    = JsonSerializeFormatter<float>::Deserialize(data.at("rotate"));
	scale     = JsonSerializeFormatter<Vector2f>::Deserialize(data.at("scale"));
}

////////////////////////////////////////////////////////////////////////////////////////////
// RectTransform structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void RectTransform::SetImGuiCommand(float granularityTranslate, float granularityScale) {
	SxImGui::DragVector2("translate", &translate.x, granularityTranslate);
	ImGui::SliderAngle("rotate",      &rotate);
	SxImGui::DragVector2("scale",     &scale.x, granularityScale, 0.0f, std::numeric_limits<float>::max());

	ImGui::Separator();

	SxImGui::DragVector2("pivot",  &pivot.x, 0.01f, 0.0f, 1.0f, "%.2f");
	SxImGui::DragFloat("priority", &priority, 0.01f, 0.0f);
}

Matrix4x4 RectTransform::ToMatrixPivot() const {
	Matrix4x4 mat = Matrix4x4::Identity();

	mat *= Matrix4x4::MakeTranslate({ -pivot.x, -pivot.y, 0.0f });
	mat *= Matrix4x4::MakeScale({ scale.x, scale.y, 1.0f });
	mat *= Matrix4x4::MakeRotateZ(rotate);
	mat *= Matrix4x4::MakeTranslate({ pivot.x, pivot.y, 0.0f });
	mat *= Matrix4x4::MakeTranslate({ translate.x, translate.y, priority });

	return mat;
}

Matrix4x4 RectTransform::ToMatrix() const {
	return Matrix4x4::MakeAffine({ scale.x, scale.y, 1.0f }, { 0.0f, 0.0f, rotate }, { translate.x, translate.y, priority });
}

json RectTransform::ParseToJson() const {
	json data = json::object();
	data["translate"] = JsonSerializeFormatter<Vector2f>::Serialize(translate);
	data["rotate"]    = JsonSerializeFormatter<float>::Serialize(rotate);
	data["scale"]     = JsonSerializeFormatter<Vector2f>::Serialize(scale);
	data["pivot"]     = JsonSerializeFormatter<Vector2f>::Serialize(pivot);
	data["priority"]  = JsonSerializeFormatter<float>::Serialize(priority);
	return data;
}

void RectTransform::InputJson(const json& data) {
	translate = JsonSerializeFormatter<Vector2f>::Deserialize(data.at("translate"));
	rotate    = JsonSerializeFormatter<float>::Deserialize(data.at("rotate"));
	scale     = JsonSerializeFormatter<Vector2f>::Deserialize(data.at("scale"));
	pivot     = JsonSerializeFormatter<Vector2f>::Deserialize(data.at("pivot"));
	priority  = JsonSerializeFormatter<float>::Deserialize(data.at("priority"));
}
