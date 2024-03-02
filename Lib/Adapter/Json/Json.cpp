#include "Json.h"

const std::string JsonAdapter::directory_ = "./Resources/Json/";

Json JsonAdapter::ReadJson(const std::string& path) {
	Json result;

	// ファイルパスの生成
	std::string filePath = directory_ + path;

	// ファイル
	std::ifstream ifs;
	ifs.open(filePath);

	assert(!ifs.fail());

	ifs >> result;

	ifs.close();

	return result;
}

void JsonAdapter::WriteJson(const std::string& path, const Json& data) {
	// ファイルパス生成
	std::string filePath = directory_ + path;

	std::ofstream ofs;
	ofs.open(filePath);

	assert(!ofs.fail());

	ofs << std::setw(4) << data << std::endl;
	ofs.close();
}