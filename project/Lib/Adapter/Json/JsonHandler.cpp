#include "JsonHandler.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <fstream>

////////////////////////////////////////////////////////////////////////////////////////////
// JsonHandler class methods
////////////////////////////////////////////////////////////////////////////////////////////

bool JsonHandler::LoadFromJson(const std::filesystem::path& filepath, json& data) {

	if (!std::filesystem::exists(filepath)) {
		return false;
	}

	std::ifstream ifs(filepath);

	if (!ifs.is_open()) {
		return false;
	}

	data = json::parse(ifs);
	return true;
}

json JsonHandler::LoadFromJson(const std::filesystem::path& filepath) {
	if (!std::filesystem::exists(filepath)) {
		throw std::runtime_error("JsonHandler::LoadFromJson() : file open error.");
	}

	std::ifstream ifs(filepath);

	if (!ifs.is_open()) {
		throw std::runtime_error("JsonHandler::LoadFromJson() : file open error.");
	}

	json data = json::parse(ifs);
	return data;
}

void JsonHandler::WriteToJson(const std::filesystem::path& filepath, const json& data) {
	std::ofstream ofs(filepath);

	if (!ofs.is_open()) {
		throw std::runtime_error("JsonHandler::WriteToJson() : file open error.");
		return;
	}

	ofs << std::setfill('\t') << std::setw(1) << data << std::endl;
	ofs.close();
}

bool JsonHandler::CheckExist(const std::filesystem::path& filepath) {
	return std::filesystem::exists(filepath);
}
