#include "IJsonSerializer.h"

json IJsonSerializer::ToJson(const Vector2f& v) {
	return { {"x", v.x}, {"y", v.y} };
}

Vector2f IJsonSerializer::ToVector2f(const json& data) {
	return { data["x"], data["y"] };
}

json IJsonSerializer::ToJson(const Vector3f& v) {
	return { {"x", v.x}, {"y", v.y}, {"z", v.z} };
}

Vector3f IJsonSerializer::ToVector3f(const json& data) {
	return { data["x"], data["y"], data["z"] };
}

json IJsonSerializer::ToJson(const Vector4f& v) {
	return { {"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w} };
}

Vector4f IJsonSerializer::ToVector4f(const json& data) {
	return { data["x"], data["y"], data["z"], data["w"] };
}

json IJsonSerializer::ToJson(const Color4f& c) {
	return { {"r", c.r}, {"g", c.g}, {"b", c.b}, {"a", c.a} };
}

Color4f IJsonSerializer::ToColor4f(const json& data) {
	return { data["r"], data["g"], data["b"], data["a"] };
}

json IJsonSerializer::ToJson(const Quaternion& q) {
	return { {"x", q.x}, {"y", q.y}, {"z", q.z}, {"w", q.w} };
}

Quaternion IJsonSerializer::ToQuaternion(const json& data) {
	return { data["x"], data["y"], data["z"], data["w"] };
}
