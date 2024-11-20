#pragma once

//-----------------------------------------------------------------------------------------
// Include
//-----------------------------------------------------------------------------------------
// externals
#include <json.hpp>

// c++
#include <fstream>
#include <cassert>

//=========================================================================================
// using
//=========================================================================================
using json = nlohmann::json;

////////////////////////////////////////////////////////////////////////////////////////////
// JsonAdapter class
////////////////////////////////////////////////////////////////////////////////////////////
class JsonAdapter {
public:

	//=========================================================================================
	// public variables
	//=========================================================================================

	static const std::string directory_;

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief Jsonファイル読み込み
	//! 
	//! @param[in] path ファイルパス. directory_ + path
	//! 
	//! @return Json型を返却
	static json LoadJson(const std::string& path);

	//! @brief Jsonファイル書き込み
	//! 
	//! @param[in] path ファイルパス. directory_ + path
	//! @param[in] data Jsonデータ
	static void WriteJson(const std::string& path, const json& data);

	//! @brief Jsonファイル上書き
	//! 
	//! @param[in] path ファイルパス. directory_ + path
	//! @param[in] data Jsonデータ
	static void OverwriteJson(const std::string& path, const json& data);

};