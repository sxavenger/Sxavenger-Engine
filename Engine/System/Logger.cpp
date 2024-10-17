#include "Logger.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <comdef.h>
#include <cassert>
#include <sstream>
#include <iomanip>

//* windows
#include <windows.h>


//-----------------------------------------------------------------------------------------
// methods
//-----------------------------------------------------------------------------------------

std::wstring ToWstring(const std::string& str) {
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

void Log(const std::string& log) {
	std::string output = log + "\n";
	OutputDebugStringA(output.c_str());
	ExternalLogger::Write(log);
}

void Log(const std::wstring& logW) {
	std::string str = ToString(logW);
	Log(str);
}

void Assert(bool expresion, const std::string& detail, const std::source_location& location) {
	if (expresion) {
		return;
	}

	std::ostringstream message;
	message << "[location]\n";
	message << " filename: " << location.file_name()     << "\n";
	message << " function: " << location.function_name() << "\n";
	message << " line:     " << location.line()          << "\n";

	if (!detail.empty()) {
		message << "\n[details]\n";
		message << " " << detail << "\n";
	}

	ExternalLogger::Write(message.str());

	OutputDebugStringA(
		std::string("\nError: Sxavenger Engine assertion\n\n" + message.str() + "\n").c_str()
	);

	MessageBoxA(
		NULL,
		message.str().c_str(),
		"Sxavenger Engine assertion",
		MB_TASKMODAL | MB_ICONHAND
	);

	__debugbreak();
}

void AssertW(bool expresion, const std::wstring& detail, const std::source_location& location) {
	if (expresion) {
		return;
	}

	std::wostringstream message;
	message << "[location]\n";
	message << " filename: " << location.file_name() << "\n";
	message << " function: " << location.function_name() << "\n";
	message << " line:     " << location.line() << "\n";

	if (!detail.empty()) {
		message << "\n[details]\n";
		message << " " << detail << "\n";
	}

	ExternalLogger::Write(ToString(message.str()));
	
	OutputDebugStringW(
		std::wstring(L"\nError: Sxavenger Engine assertion\n\n" + message.str() + L"\n").c_str()
	);

	MessageBoxW(
		NULL,
		message.str().c_str(),
		L"Sxavenger Engine assertion",
		MB_TASKMODAL | MB_ICONHAND
	);

	__debugbreak();
}

////////////////////////////////////////////////////////////////////////////////////////////
// ExternalLogger class
////////////////////////////////////////////////////////////////////////////////////////////

bool ExternalLogger::isOutput_ = false;

const std::string ExternalLogger::filename_ = "SxavengerEngineLog";
std::ofstream ExternalLogger::file_;

void ExternalLogger::Open() {
	if (!isOutput_) { return; }

	std::string label = filename_ + ".md"; // markdown形式で書き込み 
	file_.open(label);
}

void ExternalLogger::Close() {
	file_.close();
}

void ExternalLogger::Write(const std::string& log) {
	if (!isOutput_) { return; }

	file_ << log + "\n";
	file_.flush();
}
