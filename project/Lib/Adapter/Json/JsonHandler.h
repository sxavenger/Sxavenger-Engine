#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <json.hpp>

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

using json = nlohmann::json;

////////////////////////////////////////////////////////////////////////////////////////////
// JsonHandler class
////////////////////////////////////////////////////////////////////////////////////////////
class JsonHandler {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static bool LoadFromJson(const std::filesystem::path& filepath, json& data);
	static json LoadFromJson(const std::filesystem::path& filepath);

	static void WriteToJson(const std::filesystem::path& filepath, const json& data);

	static bool CheckExist(const std::filesystem::path& filepath);

};
