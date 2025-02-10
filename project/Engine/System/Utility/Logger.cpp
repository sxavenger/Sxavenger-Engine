#include "Logger.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerConfig.h>

//* c++
#include <comdef.h>
#include <sstream>
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////
// Convert string methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string ToString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

std::wstring ToWString(const std::string& str) {

	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;

}

//=========================================================================================
// static variable
//=========================================================================================

const std::filesystem::path SxavengerLogger::filepath_ = "SxavengerEngineLog.md";

std::mutex SxavengerLogger::mutex_;

#ifdef _DEVELOPMENT
#define _OUTPUT_SXAVENGER_LOG_FILE //!< ログファイルに出力するかどうか
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerLogger class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerLogger::Init() {
#ifdef _OUTPUT_SXAVENGER_LOG_FILE
	std::ofstream file(filepath_, std::ofstream::out | std::ofstream::trunc); //!< clear file
	file << "# "  << "Sxavenger Engine Log" << "\n";
	file << "## " << "engine version: " << kSxavengerEngineVersion << "\n";
#endif
}

void SxavengerLogger::LogA(const std::string& mes) {
	std::unique_lock<std::mutex> lock(mutex_);

	OutputA(mes);

#ifdef _OUTPUT_SXAVENGER_LOG_FILE
	TextA(mes);
#endif
}

void SxavengerLogger::LogW(const std::wstring& log) {
	std::unique_lock<std::mutex> lock(mutex_);

	OutputW(log);

#ifdef _OUTPUT_SXAVENGER_LOG_FILE
	TextW(log);
#endif
}

void SxavengerLogger::ExceptionA(const std::string& label, const std::string& detail, const std::source_location& location) {
	// location
	std::ostringstream locationMes;
	locationMes << "[location]" << "  \n";
	locationMes << " filename: " << location.file_name()     << "  \n";
	locationMes << " function: " << location.function_name() << "  \n";
	locationMes << " line:     " << location.line()          << "  \n";
	OutputA("\nError: Sxavenger Engine assertion\n\n" + locationMes.str());

	// label
	std::ostringstream labelMes;
	if (!label.empty()) {
		labelMes << "[label]" << "  \n";
		labelMes << " " << label << "\n";
		OutputA(labelMes.str());
	}

	// detail
	std::ostringstream detailMes;
	if (!detail.empty()) {
		detailMes << "[detail]" << "  \n";
		detailMes << " " << detail << "\n";
		OutputA(detailMes.str());
	}

#ifdef _OUTPUT_SXAVENGER_LOG_FILE
	{
		std::unique_lock<std::mutex> lock(mutex_);
		TextA("> Exception");
		TextA(locationMes.str());
		TextA(labelMes.str());
		TextA(detailMes.str());
	}
#endif

	// window
	MessageBoxA(
		NULL,
		(locationMes.str() + "\n" + labelMes.str()).c_str(),
		"Sxavenger Engine assertion",
		MB_TASKMODAL | MB_ICONHAND
	);

	__debugbreak();
}

void SxavengerLogger::ExceptionW(const std::wstring& label, const std::wstring& detail, const std::source_location& location) {
	// location
	std::wostringstream locationMes;
	locationMes << "[location]" << "  \n";
	locationMes << " filename: " << location.file_name() << "  \n";
	locationMes << " function: " << location.function_name() << "  \n";
	locationMes << " line:     " << location.line() << "  \n";
	OutputW(std::wstring(L"\nError: Sxavenger Engine assertion\n\n" + locationMes.str()).c_str());

	// label
	std::wostringstream labelMes;
	if (!label.empty()) {
		labelMes << "[label]" << "  \n";
		labelMes << " " << label << "\n";
		OutputW(labelMes.str());
	}

	// detail
	std::wostringstream detailMes;
	if (!detail.empty()) {
		detailMes << "[detail]" << "  \n";
		detailMes << " " << detail << "\n";
		OutputW(detailMes.str());
	}

#ifdef _OUTPUT_SXAVENGER_LOG_FILE
	{
		std::unique_lock<std::mutex> lock(mutex_);
		TextW(L"> Exception");
		TextW(locationMes.str());
		TextW(labelMes.str());
		TextW(detailMes.str());
	}
#endif

	// window
	MessageBoxW(
		NULL,
		(locationMes.str() + L"\n" + labelMes.str()).c_str(),
		L"Sxavenger Engine assertion",
		MB_TASKMODAL | MB_ICONHAND
	);

	__debugbreak();
}

void SxavengerLogger::OutputA(const std::string& mes) {
	OutputDebugStringA(mes.c_str());
	OutputDebugStringA("\n");
}

void SxavengerLogger::OutputW(const std::wstring& mes) {
	OutputDebugStringW(mes.c_str());
	OutputDebugStringW(L"\n");
}

void SxavengerLogger::TextA(const std::string& mes) {
	std::ofstream file(filepath_, mode_);
	file << mes << "  " << "\n";
}

void SxavengerLogger::TextW(const std::wstring& mes) {
	std::wofstream file(filepath_, mode_);
	file << mes << "  " << "\n";
}

////////////////////////////////////////////////////////////////////////////////////////////
// Log methods
////////////////////////////////////////////////////////////////////////////////////////////

void Log(const std::string& log) {
	SxavengerLogger::LogA(log);
}

void Log(const std::wstring& log) {
	SxavengerLogger::LogW(log);
}

void EngineLog(const std::string& log) {
	std::string tag = "[Sxavenger Engine] >> ";
	SxavengerLogger::LogA(tag + log);
}

void EngineLog(const std::wstring& log) {
	std::wstring tag = L"[Sxavenger Engine] >> ";
	SxavengerLogger::LogW(tag + log);
}

void EngineThreadLog(const std::string& log) {
	std::ostringstream tag;
	tag << "[Sxavenger Engine] [thread id: " << std::this_thread::get_id() << "] : ";
	SxavengerLogger::LogA(tag.str() + log);
}

void EngineThreadLog(const std::wstring& log) {
	std::wostringstream tag;
	tag << "[Sxavenger Engine] [thread id: " << std::this_thread::get_id() << "] : ";
	SxavengerLogger::LogW(tag.str() + log);
}

void Assert(bool expression, const std::string& label, const std::string& detail, const std::source_location& location) {
	if (expression) {
		return;
	}

	SxavengerLogger::ExceptionA(label, detail, location);
}

void AssertW(bool expression, const std::wstring& label, const std::wstring& detail, const std::source_location& location) {
	if (expression) {
		return;
	}

	SxavengerLogger::ExceptionW(label, detail, location);
}
