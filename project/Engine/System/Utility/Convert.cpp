#include "Convert.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#include <comdef.h>

//* c++
#include <chrono>
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// Convert string methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string ToString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto size = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (size == 0) {
		return std::string();
	}

	std::string result(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), size, NULL, NULL);
	return result;
}

std::wstring ToWString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto size = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (size == 0) {
		return std::wstring();
	}

	std::wstring result(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], size);
	return result;

}

std::wstring ToWString(const std::string_view& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto size = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (size == 0) {
		return std::wstring();
	}

	std::wstring result(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], size);
	return result;
}

std::string GetCurrentDateTime(char separatorDate, char separatorDateTime, char separatorTime) {

	auto now = std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() };
	auto local = now.get_local_time();

	std::string fmt = std::format(
		"%Y{}%m{}%d{}%H{}%M{}%S",
		separatorDate, separatorDate,
		separatorDateTime,
		separatorTime, separatorTime
	);

	return std::vformat(fmt, std::make_format_args(local));

}
