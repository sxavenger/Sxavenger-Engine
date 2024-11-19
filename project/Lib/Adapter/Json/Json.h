#pragma once

//-----------------------------------------------------------------------------------------
// Include
//-----------------------------------------------------------------------------------------
// externals
#include <json.hpp>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Quaternion.h>

// c++
#include <fstream>
#include <cassert>

//=========================================================================================
// using
//=========================================================================================
using Json = nlohmann::json;

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

	//* write and load *//

	//! @brief Jsonファイル読み込み
	//! 
	//! @param[in] path ファイルパス. directory_ + path
	//! 
	//! @return Json型を返却
	static Json LoadJson(const std::string& path);


	static bool TryLoadJson(const std::string& path, Json& data);

	//! @brief Jsonファイル書き込み
	//! 
	//! @param[in] path ファイルパス. directory_ + path
	//! @param[in] data Jsonデータ
	static void WriteJson(const std::string& path, const Json& data);

	//! @brief Jsonファイル上書き
	//! 
	//! @param[in] path ファイルパス. directory_ + path
	//! @param[in] data Jsonデータ
	static void OverwriteJson(const std::string& path, const Json& data);

	//* serialize *//

	static Json ToJson(const Vector2f& v);
	static Json ToJson(const Vector3f& v);
	static Json ToJson(const Vector4f& v);
	static Json ToJson(const Quaternion& q);
	static Json ToJson(const Color4f& c);

	static Vector2f ToVector2f(const Json& data);
	static Vector3f ToVector3f(const Json& data);
	static Vector4f ToVector4f(const Json& data);
	static Quaternion ToQuaternion(const Json& data);
	static Color4f ToColor4f(const Json& data);

};