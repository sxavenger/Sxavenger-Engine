#include "Transform2d.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Transform2d structure methods
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
