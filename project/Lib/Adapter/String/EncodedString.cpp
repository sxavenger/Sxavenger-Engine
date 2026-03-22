#include "EncodedString.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#include <comdef.h>

//* sxl
#include <Lib/Sxl/Exception.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EncodedString class methods
////////////////////////////////////////////////////////////////////////////////////////////

EncodedString::Type EncodedString::GetEncodedType() const {
	return static_cast<Type>(string_.index());
}

std::string EncodedString::GetString() const {
	switch (EncodedString::GetEncodedType()) {
		case Type::String:
			return std::get<std::string>(string_);

		case Type::WString:
			return EncodedString::Convert(std::get<std::wstring>(string_));

		default:
			return std::string{};
	}
}

std::wstring EncodedString::GetWString() const {
	switch (EncodedString::GetEncodedType()) {
		case Type::String:
			return EncodedString::Convert(std::get<std::string>(string_));

		case Type::WString:
			return std::get<std::wstring>(string_);

		default:
			return std::wstring{};
	}
}

const std::string& EncodedString::AsString() const {
	if (EncodedString::GetEncodedType() != Type::String) {
		Sxl::Exception::RuntimeError("encoded string is not string type.");
	}

	return std::get<std::string>(string_);
}

const std::wstring& EncodedString::AsWString() const {
	if (EncodedString::GetEncodedType() != Type::WString) {
		Sxl::Exception::RuntimeError("encoded string is not wstring type.");
	}

	return std::get<std::wstring>(string_);
}

std::string EncodedString::Convert(const std::wstring_view& wstr) {

	if (wstr.empty()) {
		return std::string{};
	}

	auto size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);

	if (size == 0) {
		return std::string{};
	}

	//!< stringへの変換
	std::string string(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), string.data(), size, NULL, NULL);

	return string;
}

std::wstring EncodedString::Convert(const std::string_view& str) {

	if (str.empty()) {
		return std::wstring{};
	}

	auto size = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0);

	if (size == 0) {
		return std::wstring{};
	}

	//!< wstringへの変換
	std::wstring wstring(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), wstring.data(), size);

	return wstring;
}
