#include "JsonAdapter.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerDirectory.h>
#include <Engine/System/Utility/Logger.h>

//* c++
#include <fstream>

////////////////////////////////////////////////////////////////////////////////////////////
// JsonAdapter class methods
////////////////////////////////////////////////////////////////////////////////////////////

json JsonAdapter::LoadFromJson(const std::filesystem::path& filepath) {

	json result;

	// ファイル
	std::ifstream ifs(filepath);
	Assert(!ifs.fail(), "JsonAdapter failed open file. filepath: " + filepath.string());

	ifs >> result;

	ifs.close();

	return result;
}

bool JsonAdapter::TryLoadFromJson(const std::filesystem::path& filepath, json& out) {
	// ファイル
	std::ifstream ifs(filepath);

	if (ifs.fail()) {
		return false;
	}

	ifs >> out;

	ifs.close();

	return true;
}

void JsonAdapter::WriteToJson(const std::filesystem::path& filepath, const json& data) {
	std::ofstream ofs(filepath);
	ofs << std::setfill('\t') << std::setw(1) << data << std::endl;
	ofs.close();
}
