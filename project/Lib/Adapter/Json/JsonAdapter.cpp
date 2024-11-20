#include "JsonAdapter.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerDirectory.h>
#include <Engine/System/Utility/Logger.h>

//* c++
#include <fstream>

//=========================================================================================
// static variables
//=========================================================================================

const std::string JsonAdapter::kDirectory_ = kJsonDirectory;

////////////////////////////////////////////////////////////////////////////////////////////
// JsonAdapter class methods
////////////////////////////////////////////////////////////////////////////////////////////

json JsonAdapter::LoadFromJson(const std::string& filename) {
	json result;

	// ファイルパスの生成
	std::string filepath = kDirectory_ + filename;

	// ファイル
	std::ifstream ifs(filepath);
	Assert(!ifs.fail(), "JsonAdapter failed open file. filename: " + filename);

	ifs >> result;

	ifs.close();

	return result;
}

bool JsonAdapter::TryLoadFromJson(const std::string& filename, json& out) {
	// ファイルパスの生成
	std::string filepath = kDirectory_ + filename;

	// ファイル
	std::ifstream ifs(filepath);

	if (ifs.fail()) {
		return false;
	}

	ifs >> out;

	ifs.close();

	return true;
}

void JsonAdapter::WriteToJson(const std::string& filename, const json& data) {
	// ファイルパスの生成
	std::string filepath = kDirectory_ + filename;

	std::ofstream ofs(filepath);
	ofs << std::setfill('\t') << std::setw(1) << data << std::endl;
	ofs.close();
}