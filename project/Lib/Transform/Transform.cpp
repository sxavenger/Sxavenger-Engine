#include "Transform.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////
// EulerTransform structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Matrix4x4 EulerTransform::ToMatrix() const {
	return Matrix4x4::MakeAffine(scale, rotate, translate);
}

////////////////////////////////////////////////////////////////////////////////////////////
// QuaternionTransform structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Matrix4x4 QuaternionTransform::ToMatrix() const {
	return Matrix4x4::MakeAffine(scale, rotate, translate);
}

Vector3f QuaternionTransform::GetForward() const {
	return Quaternion::RotateVector(kForward3<>, rotate);
}

json QuaternionTransform::Serialize() const {
	json data = json::object();

	data["translate"] = JsonSerializeFormatter<Vector3f>::Serialize(translate);
	data["rotate"]    = JsonSerializeFormatter<Quaternion>::Serialize(rotate);
	data["scale"]     = JsonSerializeFormatter<Vector3f>::Serialize(scale);

	return data;
}

QuaternionTransform QuaternionTransform::Deserialize(const json& data) {
	QuaternionTransform transform = {};

	transform.translate = JsonSerializeFormatter<Vector3f>::Deserialize(data.at("translate"));
	transform.rotate    = JsonSerializeFormatter<Quaternion>::Deserialize(data.at("rotate"));
	transform.scale     = JsonSerializeFormatter<Vector3f>::Deserialize(data.at("scale"));

	return transform;
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

Matrix4x4 Transform2d::ToMatrix() const {
	return Matrix4x4::MakeAffine({ scale.x, scale.y, 0.0f }, { 0.0f, 0.0f, rotate }, { translate.x, translate.y, 0.0f });
}

json Transform2d::Serialize() const {
	json data = json::object();
	data["translate"] = JsonSerializeFormatter<Vector2f>::Serialize(translate);
	data["rotate"]    = JsonSerializeFormatter<float>::Serialize(rotate);
	data["scale"]     = JsonSerializeFormatter<Vector2f>::Serialize(scale);
	return data;
}

Transform2d Transform2d::Deserialize(const json& data) {
	Transform2d transform = {};
	transform.translate = JsonSerializeFormatter<Vector2f>::Deserialize(data.at("translate"));
	transform.rotate    = JsonSerializeFormatter<float>::Deserialize(data.at("rotate"));
	transform.scale     = JsonSerializeFormatter<Vector2f>::Deserialize(data.at("scale"));
	return transform;
}

////////////////////////////////////////////////////////////////////////////////////////////
// RectTransform structure methods
////////////////////////////////////////////////////////////////////////////////////////////

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
