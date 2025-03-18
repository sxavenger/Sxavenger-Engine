#include "IJsonSerializer.h"

json GeometryJsonSerializer::ToJson(const Vector2f& v) {
	return { {"x", v.x}, {"y", v.y} };
}

Vector2f GeometryJsonSerializer::JsonToVector2f(const json& data) {
	return { data["x"], data["y"] };
}

json GeometryJsonSerializer::ToJson(const Vector3f& v) {
	return { {"x", v.x}, {"y", v.y}, {"z", v.z} };
}

Vector3f GeometryJsonSerializer::JsonToVector3f(const json& data) {
	return { data["x"], data["y"], data["z"] };
}

json GeometryJsonSerializer::ToJson(const Vector4f& v) {
	return { {"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w} };
}

Vector4f GeometryJsonSerializer::JsonToVector4f(const json& data) {
	return { data["x"], data["y"], data["z"], data["w"] };
}

json GeometryJsonSerializer::ToJson(const Color4f& c) {
	return { {"r", c.r}, {"g", c.g}, {"b", c.b}, {"a", c.a} };
}

Color4f GeometryJsonSerializer::JsonToColor4f(const json& data) {
	return { data["r"], data["g"], data["b"], data["a"] };
}

json GeometryJsonSerializer::ToJson(const Color3f& c) {
	return { { "r", c.r }, { "g", c.g }, { "b", c.b } };
}

Color3f GeometryJsonSerializer::JsonToColor3f(const json& data) {
	return { data["r"], data["g"], data["b"] };
}

json GeometryJsonSerializer::ToJson(const Quaternion& q) {
	return { {"x", q.x}, {"y", q.y}, {"z", q.z}, {"w", q.w} };
}

Quaternion GeometryJsonSerializer::JsonToQuaternion(const json& data) {
	return { data["x"], data["y"], data["z"], data["w"] };
}
