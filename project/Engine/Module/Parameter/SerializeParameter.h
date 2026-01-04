#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Configuration/Configuration.h>

//* lib
#include <Lib/Adapter/Json/JsonSerializer.h>
#include <Lib/Adapter/Json/JsonHandler.h>

//* c++
#include <string>
#include <filesystem>
#include <source_location>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SerializeParameter class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ParameterをJsonファイルに保存/読み込みするためのクラス.
template <typename T>
class SerializeParameter {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SerializeParameter() = delete;
	SerializeParameter(const std::string& label, const T& value = T{}, const std::source_location& location = std::source_location::current());

	//* 保存/読み込み *//

	void Read();

	void Save() const;

	//* operator [accessor] *//

	const T& operator*() const { return value_; }
	operator const T& () const { return value_; }

	//* getter *//

	T& Get() { return value_; }
	const T& Get() const { return value_; }

	const std::string& Label() const { return label_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	T value_;
	T default_ = T{};

	const std::string label_;
	const std::filesystem::path current_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	std::filesystem::path GetFilepath() const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// SerializeParameter class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline SerializeParameter<T>::SerializeParameter(const std::string& label, const T& value, const std::source_location& location)
	: label_(label), current_(std::filesystem::path(location.file_name()).filename()) {
	default_ = value;
	Read();
}

template <typename T>
inline void SerializeParameter<T>::Read() {
	std::filesystem::path filepath = GetFilepath();

	if (!std::filesystem::exists(filepath)) {
		value_ = default_;
		return;
	}

	json data = JsonHandler::LoadFromJson(filepath);

	if (!data.contains(label_)) {
		value_ = default_;
		return;
	}

	value_ = JsonSerializeFormatter<T>::Deserialize(data[label_]);
}

template <typename T>
inline void SerializeParameter<T>::Save() const {
	json data = json::object();
	data[label_] = JsonSerializeFormatter<T>::Serialize(value_);
	JsonHandler::OverwriteToJson(GetFilepath(), data);
}

template <typename T>
inline std::filesystem::path SerializeParameter<T>::GetFilepath() const {
	return kAssetsDirectory / "Parameter" / std::filesystem::path(current_).replace_extension(".json");
}

SXAVENGER_ENGINE_NAMESPACE_END


