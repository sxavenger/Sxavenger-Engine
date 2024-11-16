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

void JsonAdapter::WriteJson(const std::string& path, const Json& data) {
	// ファイルパス生成
	std::string filePath = directory_ + path;

	std::ofstream ofs;
	ofs.open(filePath);

	/*Assert(!ofs.fail());*/

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