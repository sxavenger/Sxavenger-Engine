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
// JsonAdapter class
////////////////////////////////////////////////////////////////////////////////////////////
class JsonAdapter {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief Jsonファイル読み込み
	static json LoadFromJson(const std::filesystem::path& filepath);

	//! @brief Jsonファイル読み込み
	//! @retval true  成功
	//! @retval false 失敗
	static bool LoadFromJson(const std::filesystem::path& filepath, json& out);

	//! @brief Jsonファイルに書き込み
	static void WriteToJson(const std::filesystem::path& filepath, const json& data);

private:
};
