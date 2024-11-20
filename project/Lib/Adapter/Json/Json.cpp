#include "json.h"

//=========================================================================================
// static variables
//=========================================================================================

const std::string JsonAdapter::directory_ = "./resources/json/";

////////////////////////////////////////////////////////////////////////////////////////////
// JsonAdapter methods
////////////////////////////////////////////////////////////////////////////////////////////

json JsonAdapter::LoadJson(const std::string& path) {
	json result;

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

void JsonAdapter::WriteJson(const std::string& path, const json& data) {
	// ファイルパス生成
	std::string filePath = directory_ + path;

	std::ofstream ofs;
	ofs.open(filePath);

	/*Assert(!ofs.fail());*/

	ofs << std::setfill('\t') << std::setw(1) << data << std::endl;
	ofs.close();
}

void JsonAdapter::OverwriteJson(const std::string& path, const json& data) {

	std::string filePath = directory_ + path;

	json saveData;

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