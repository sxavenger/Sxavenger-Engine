#include "TransformQuaternion.h"

////////////////////////////////////////////////////////////////////////////////////////////
// TransformQuaternion structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f TransformQuaternion::GetForward() const {
	return Quaternion::RotateVector(kForward3<>, rotate);
}

Transformation TransformQuaternion::ToTransformation() const {
	return Transformation::MakeAffine(scale, rotate, translate);
}

Matrix4x4 TransformQuaternion::ToMatrix() const {
	return ToTransformation().ToMatrix();
}

json TransformQuaternion::Serialize() const {
	json data = json::object();
	data["translate"] = JsonSerializeFormatter<Vector3f>::Serialize(translate);
	data["rotate"]    = JsonSerializeFormatter<Quaternion>::Serialize(rotate);
	data["scale"]     = JsonSerializeFormatter<Vector3f>::Serialize(scale);

	return data;
}

TransformQuaternion TransformQuaternion::Deserialize(const json& data) {
	TransformQuaternion transform = {};
	transform.translate = JsonSerializeFormatter<Vector3f>::Deserialize(data.at("translate"));
	transform.rotate    = JsonSerializeFormatter<Quaternion>::Deserialize(data.at("rotate"));
	transform.scale     = JsonSerializeFormatter<Vector3f>::Deserialize(data.at("scale"));

	return transform;
}
