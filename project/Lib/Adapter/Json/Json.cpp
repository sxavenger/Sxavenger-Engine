#include "Json.h"

//=========================================================================================
// static variables
//=========================================================================================

const std::string JsonAdapter::directory_ = "./resources/Json/";

////////////////////////////////////////////////////////////////////////////////////////////
// JsonAdapter methods
////////////////////////////////////////////////////////////////////////////////////////////

Json JsonAdapter::LoadJson(const std::string& path) {
	Json result;

	// ファイルパスの生成
	std::string filePath = directory_ + path;

	// ファイル
	std::ifstream ifs;
	ifs.open(filePath);

	assert(!ifs.fail());

	ifs >> result;

	ifs.close();

	return std::move(result);
}

bool JsonAdapter::TryLoadJson(const std::string& path, Json& data) {

	// ファイルパスの生成
	std::string filepath = directory_ + path;

	std::ifstream ifs(filepath);
	if (ifs.fail()) {
		return false;
	}

	ifs >> data;
	ifs.close();

	return true;
}

void JsonAdapter::WriteJson(const std::string& path, const Json& data) {
	// ファイルパス生成
	std::string filePath = directory_ + path;

	std::ofstream ofs(filePath);
	ofs << std::setfill('\t') << std::setw(1) << data << std::endl;
	ofs.close();
}

void JsonAdapter::OverwriteJson(const std::string& path, const Json& data) {

	std::string filePath = directory_ + path;

	Json saveData;

	// 旧データの回収
	std::ifstream ifs(filePath);
	if (ifs.is_open()) {
		ifs >> saveData;
		ifs.close();
	}
	
	// 新データの書き込み
	saveData.update(data);

	std::ofstream ofs(filePath);
	ofs << std::setfill('\t') << std::setw(1) << data << std::endl;
	ofs.close();
}

Json JsonAdapter::ToJson(const Vector2f& v) {
	return { {"x", v.x}, {"y", v.y} };
}

Json JsonAdapter::ToJson(const Vector3f& v) {
	return { {"x", v.x}, {"y", v.y}, {"z", v.z} };
}

Json JsonAdapter::ToJson(const Vector4f& v) {
	return { {"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w} };
}

Json JsonAdapter::ToJson(const Quaternion& q) {
	return { {"x", q.x}, {"y", q.y}, {"z", q.z}, {"w", q.w} };
}

Json JsonAdapter::ToJson(const Color4f& c) {
	return { {"r", c.r}, {"g", c.g}, {"b", c.b}, {"a", c.a} };
}

Vector2f JsonAdapter::ToVector2f(const Json& data) {
	return { data["x"], data["y"] };
}

Vector3f JsonAdapter::ToVector3f(const Json& data) {
	return { data["x"], data["y"], data["z"] };
}

Vector4f JsonAdapter::ToVector4f(const Json& data) {
	return { data["x"], data["y"], data["z"], data["w"] };
}

Quaternion JsonAdapter::ToQuaternion(const Json& data) {
	return { data["x"], data["y"], data["z"], data["w"] };
}

Color4f JsonAdapter::ToColor4f(const Json& data) {
	return { data["r"], data["g"], data["b"], data["a"] };
}
