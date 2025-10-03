#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerConfig.h>

//* lib
#include <Lib/Adapter/Json/JsonSerializer.h>
#include <Lib/Adapter/Json/JsonHandler.h>

//* c++
#include <string>
#include <filesystem>
#include <typeinfo>
#include <source_location>

////////////////////////////////////////////////////////////////////////////////////////////
// SerializeParameter structure
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct SerializeParameter {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Type = T;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SerializeParameter() = delete;
	SerializeParameter(const std::string& label, const T& value = T(), const std::source_location& location = std::source_location::current())
		: label_(label), current_(std::filesystem::path(location.file_name()).filename()) {
		//!< parameterの読み込み
		defaultValue_ = value;
		Read();
	}

	void Read();

	void Save() const;

	T& Get() { return value_; }
	const T& Get() const { return value_; }

	const std::string& Label() const { return label_; }

	//* operator *//

	SerializeParameter& operator=(const T& rhs) { value_ = rhs; return *this; }
	SerializeParameter& operator=(T&& rhs) { value_ = std::move(rhs); return *this; }

	operator T& () { return value_; }
	operator const T& () const { return value_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	T value_;
	T defaultValue_ = T();

	const std::string label_;
	const std::filesystem::path current_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	std::filesystem::path GetFilepath() const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// SerializeParameter structure template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void SerializeParameter<T>::Read() {
	std::filesystem::path filepath = GetFilepath();

	if (!std::filesystem::exists(filepath)) {
		value_ = defaultValue_;
		return;
	}

	json data = JsonHandler::LoadFromJson(filepath);

	if (!data.contains(label_)) {
		value_ = defaultValue_;
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
