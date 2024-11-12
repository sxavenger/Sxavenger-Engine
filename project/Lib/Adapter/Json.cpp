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

bool JsonAdapter::LoadJson(const std::string& path, Json& data) {

	// ファイルパスの生成
	std::string filePath = directory_ + path;

	// ファイル
	std::ifstream ifs;
	ifs.open(filePath);

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

	std::ofstream ofs;
	ofs.open(filePath);

	/*Assert(!ofs.fail());*/

	ofs << std::setw(4) << data << std::endl;
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

	ofs << std::setw(4) << saveData << std::endl;

	ofs.close();
}

Json JsonAdapter::ToJson(const Vector3f& v) {
	return { {"x", v.x}, {"y", v.y}, {"z", v.z} };
}

Vector3f JsonAdapter::ToVector3(const Json& data) {
	return { data["x"], data["y"], data["z"] };
}
