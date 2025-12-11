#include "StreamLogger.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#include <comdef.h>

//* c++
#include <chrono>
#include <format>

//=========================================================================================
// static variable
//=========================================================================================

std::filesystem::path StreamLogger::filename_ = kDirectory_ / GetStreamLogFilename();

////////////////////////////////////////////////////////////////////////////////////////////
// StreamLogger class methods
////////////////////////////////////////////////////////////////////////////////////////////

void StreamLogger::Init() {
	std::filesystem::create_directories(kDirectory_); //!< directoryの作成

	std::ofstream file(filename_, std::ofstream::out | std::ofstream::trunc); //!< fileの作成
	file << "Sxavenger Engine [Sxx Engine] Stream Log" << "\n";
	file << "version: " << SXAVENGER_ENGINE_VERSION << "\n";
	file << "profile: " << _PROFILE << "\n";
}

void StreamLogger::Log(const std::string& message) {
	std::unique_lock<std::mutex> lock(mutex_);
	StreamLogger::OutputA(message);
}

void StreamLogger::Log(const std::wstring& message) {
	std::unique_lock<std::mutex> lock(mutex_);
	StreamLogger::OutputW(message);
}

NORETURN void StreamLogger::Exception(const std::string& label, const std::string& detail, const std::source_location& location) {

	{
		std::unique_lock<std::mutex> lock(mutex_);

		StreamLogger::OutputA("\n[Error ExceptionA]: Sxavenger Engine Exception.\n\n");

		// location
		std::string locationMessage = StreamLogger::GetLocationMessageA(location);
		StreamLogger::OutputConsoleA(locationMessage);

		// label
		if (!label.empty()) {
			std::string labelMessage = StreamLogger::GetTagMessageA("label", label);
			StreamLogger::OutputConsoleA(labelMessage);
		}

		// detail
		if (!detail.empty()) {
			std::string detailMessage = StreamLogger::GetTagMessageA("detail", detail);
			StreamLogger::OutputConsoleA(detailMessage);
		}

		// thread
		std::string threadMessage = StreamLogger::GetThreadMessageA(std::this_thread::get_id());
		OutputConsoleA(threadMessage);
	}

	StreamLogger::DebugBreak();
}

NORETURN void StreamLogger::Exception(const std::wstring& label, const std::wstring& detail, const std::source_location& location) {

	{
		std::unique_lock<std::mutex> lock(mutex_);

		StreamLogger::OutputW(L"\n[Error ExceptionW]: Sxavenger Engine Exception.\n\n");

		// location
		std::wstring locationMessage = StreamLogger::GetLocationMessageW(location);
		StreamLogger::OutputConsoleW(locationMessage);

		// label
		if (!label.empty()) {
			std::wstring labelMessage = StreamLogger::GetTagMessageW(L"label", label);
			StreamLogger::OutputConsoleW(labelMessage);
		}

		// detail
		if (!detail.empty()) {
			std::wstring detailMessage = StreamLogger::GetTagMessageW(L"detail", detail);
			StreamLogger::OutputConsoleW(detailMessage);
		}

		// thread
		std::wstring threadMessage = StreamLogger::GetThreadMessageW(std::this_thread::get_id());
		OutputConsoleW(threadMessage);
	}

	StreamLogger::DebugBreak();
}

void StreamLogger::EngineLog(const std::string& message) {
	std::string tag = "[Sxavenger Engine] >> ";
	StreamLogger::Log(tag + message);
}

void StreamLogger::EngineLog(const std::wstring& message) {
	std::wstring tag = L"[Sxavenger Engine] >> ";
	StreamLogger::Log(tag + message);
}

void StreamLogger::EngineThreadLog(const std::string& message) {
	std::ostringstream tag;
	tag << "[Sxavenger Engine] ";

	std::thread::id id = std::this_thread::get_id();

	if (id == kMainThreadId_) {
		tag << "[main thread] >> ";

	} else {
		tag << "[thread id: " << id << "] >> ";
	}

	StreamLogger::Log(tag.str() + message);
}

void StreamLogger::EngineThreadLog(const std::wstring& message) {
	std::wostringstream tag;
	tag << L"[Sxavenger Engine] ";

	std::thread::id id = std::this_thread::get_id();

	if (id == kMainThreadId_) {
		tag << L"[main thread] >> ";

	} else {
		tag << L"[thread id: " << id << L"] >> ";
	}

	StreamLogger::Log(tag.str() + message);
}

void StreamLogger::AssertA(bool expression, const std::string& label, const std::string& detail, const std::source_location& location) {
	if (expression) LIKELY {
		return;
	}

	StreamLogger::Exception(label, detail, location);
}

void StreamLogger::AssertW(bool expression, const std::wstring& label, const std::wstring& detail, const std::source_location& location) {
	if (expression) LIKELY {
		return;
	}

	StreamLogger::Exception(label, detail, location);
}

std::filesystem::path StreamLogger::GetStreamLogFilename() {

	std::chrono::system_clock::time_point current = std::chrono::system_clock::now();
	std::chrono::zoned_time time{ std::chrono::current_zone(), current };

	return std::format("{:%Y-%m-%d_%H-%M-%S}.log", time);
}

void StreamLogger::OutputConsoleA(const std::string& message) {
	OutputDebugStringA(message.c_str());
	OutputDebugStringA("\n");
}

void StreamLogger::OutputConsoleW(const std::wstring& message) {
	OutputDebugStringW(message.c_str());
	OutputDebugStringW(L"\n");
}

void StreamLogger::OutputFileA(const std::string& message) {
	std::ofstream file(filename_, mode_);
	file << message << "\n";
}

void StreamLogger::OutputFileW(const std::wstring& message) {
	std::wofstream file(filename_, mode_);
	file << message << "\n";
}

void StreamLogger::OutputA(const std::string& message) {
	OutputConsoleA(message);
	OutputFileA(message);
}

void StreamLogger::OutputW(const std::wstring& message) {
	OutputConsoleW(message);
	OutputFileW(message);
}

NORETURN void StreamLogger::DebugBreak() {
	__debugbreak(); //!< windows用 debug break
}

std::string StreamLogger::GetLocationMessageA(const std::source_location& location) {
	std::ostringstream message;
	message << "[location]"  << "\n";
	message << " filename: " << location.file_name()     << "\n";
	message << " function: " << location.function_name() << "\n";
	message << " line:     " << location.line()          << "\n";

	return message.str();
}

std::wstring StreamLogger::GetLocationMessageW(const std::source_location& location) {
	std::wostringstream message;
	message << L"[location]"  << L" \n";
	message << L" filename: " << location.file_name()     << L"\n";
	message << L" function: " << location.function_name() << L"\n";
	message << L" line:     " << location.line()          << L"\n";

	return message.str();
}

std::string StreamLogger::GetThreadMessageA(const std::thread::id id) {
	std::ostringstream message;
	message << "[thread]" << "\n";

	if (id == kMainThreadId_) {
		message << " main thread\n";

	} else {
		message << " id: " << id << "\n";
	}

	return message.str();
}

std::wstring StreamLogger::GetThreadMessageW(const std::thread::id id) {
	std::wostringstream message;
	message << L"[thread]" << L"\n";

	if (id == kMainThreadId_) {
		message << L" main thread\n";

	} else {
		message << L" id: " << id << L"\n";
	}

	return message.str();
}

std::string StreamLogger::GetTagMessageA(const std::string& tag, const std::string& text) {
	std::ostringstream message;
	message << "[" << tag << "]" << "\n";
	message << " " << text << "\n";

	return message.str();
}

std::wstring StreamLogger::GetTagMessageW(const std::wstring& tag, const std::wstring& text) {
	std::wostringstream message;
	message << L"[" << tag << L"]" << L"\n";
	message << L" " << text << L"\n";

	return message.str();
}
