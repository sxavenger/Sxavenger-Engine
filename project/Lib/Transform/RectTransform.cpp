#include "RectTransform.h"

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

json RectTransform::Serialize() const {
	json data = json::object();
	data["translate"] = JsonSerializeFormatter<Vector2f>::Serialize(translate);
	data["rotate"]    = JsonSerializeFormatter<float>::Serialize(rotate);
	data["scale"]     = JsonSerializeFormatter<Vector2f>::Serialize(scale);
	data["pivot"]     = JsonSerializeFormatter<Vector2f>::Serialize(pivot);
	data["priority"]  = JsonSerializeFormatter<float>::Serialize(priority);

	return data;
}

RectTransform RectTransform::Deserialize(const json& data) {
	RectTransform t;
	t.translate = JsonSerializeFormatter<Vector2f>::Deserialize(data.at("translate"));
	t.rotate    = JsonSerializeFormatter<float>::Deserialize(data.at("rotate"));
	t.scale     = JsonSerializeFormatter<Vector2f>::Deserialize(data.at("scale"));
	t.pivot     = JsonSerializeFormatter<Vector2f>::Deserialize(data.at("pivot"));
	t.priority  = JsonSerializeFormatter<float>::Deserialize(data.at("priority"));

	return t;
}
