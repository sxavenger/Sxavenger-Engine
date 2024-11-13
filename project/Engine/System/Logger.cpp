#include "Logger.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <comdef.h>
#include <sstream>

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

////////////////////////////////////////////////////////////////////////////////////////////
// Log methods
////////////////////////////////////////////////////////////////////////////////////////////

void Log(const std::string& log) {
	std::string output = log + "\n";
	OutputDebugStringA(output.c_str());
}

void Log(const std::wstring& log) {
	std::wstring output = log + L"\n";
	OutputDebugStringW(output.c_str());
}

void EngineLog(const std::string& log) {
	std::string tag = "## Sxavenger Engine >> ";
	OutputDebugStringA(tag.c_str());
	Log(log);
}

void EngineLog(const std::wstring& log) {
	std::string tag = "## Sxavenger Engine >> ";
	OutputDebugStringA(tag.c_str());
	Log(log);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Assert methods
////////////////////////////////////////////////////////////////////////////////////////////

void Assert(bool expresion, const std::string& detail, const std::source_location& location) {
	if (expresion) {
		return;
	}

	// location message
	std::ostringstream locationMes;
	locationMes << "[location]\n";
	locationMes << " filename: " << location.file_name()     << "\n";
	locationMes << " function: " << location.function_name() << "\n";
	locationMes << " line:     " << location.line()          << "\n";

	Log(std::string("\nError: Sxavenger Engine assertion\n\n" + locationMes.str()).c_str());

	// detail message
	std::ostringstream detailMes;

	if (!detail.empty()) {
		detailMes << "[details]\n";
		detailMes << " " << detail << "\n";

		Log(detailMes.str());
		//ExternalLogger::Write(detailMes.str());
	}

	MessageBoxA(
		NULL,
		(locationMes.str() + "\n" + detailMes.str()).c_str(),
		"Sxavenger Engine assertion",
		MB_TASKMODAL | MB_ICONHAND
	);

	__debugbreak();
}

void AssertW(bool expresion, const std::wstring& detail, const std::source_location& location) {
	if (expresion) {
		return;
	}

	// location message
	std::wostringstream locationMes;
	locationMes << "[location]\n";
	locationMes << " filename: " << location.file_name()     << "\n";
	locationMes << " function: " << location.function_name() << "\n";
	locationMes << " line:     " << location.line()          << "\n";

	Log(std::wstring(L"\nError: Sxavenger Engine assertion\n\n" + locationMes.str()).c_str());

	// detail message
	std::wostringstream detailMes;

	if (!detail.empty()) {
		detailMes << "[details]\n";
		detailMes << " " << detail << "\n";

		Log(detailMes.str());
		//ExternalLogger::Write(detailMes.str());
	}

	MessageBoxW(
		NULL,
		(locationMes.str() + L"\n" + detailMes.str()).c_str(),
		L"Sxavenger Engine assertion",
		MB_TASKMODAL | MB_ICONHAND
	);

	__debugbreak();
}