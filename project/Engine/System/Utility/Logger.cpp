#include "Logger.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerConfig.h>

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <comdef.h>
#include <sstream>
#include <thread>

//=========================================================================================
// static variable
//=========================================================================================

//* thread safety *//
std::mutex SxavengerLogger::mutex_;

//* file output parameters *//
const std::filesystem::path SxavengerLogger::filepath_ = "SxavengerEngineLog.md";
const std::ofstream::openmode SxavengerLogger::mode_   = std::ofstream::out | std::ofstream::app;

//* log stacks *//
SxavengerLogger::Container SxavengerLogger::stacks_;

#ifdef _DEVELOPMENT
#define _OUTPUT_SXAVENGER_LOG_FILE //!< ログファイルに出力するかどうか
#define _LOG_STACK                 //!< ログスタックを有効にするかどうか
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
	locationMes << " filename: " << location.file_name() << "  \n";
	locationMes << " function: " << location.function_name() << "  \n";
	locationMes << " line:     " << location.line() << "  \n";
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

	// thread
	std::ostringstream threadMes;
	threadMes << "[thread]" << "  \n";
	threadMes << " id: " << std::this_thread::get_id() << "\n";
	OutputA(threadMes.str());

#ifdef _OUTPUT_SXAVENGER_LOG_FILE
	{
		std::unique_lock<std::mutex> lock(mutex_);
		TextA("> Exception");
		TextA(locationMes.str());
		TextA(labelMes.str());
		TextA(detailMes.str());
		TextA(threadMes.str());
	}
#endif

	// window
	MessageBoxA(
		NULL,
		(locationMes.str() + "\n" + labelMes.str()).c_str(),
		"Sxavenger Engine assertion",
		MB_TASKMODAL | MB_ICONHAND | MB_TOPMOST
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

	// thread
	std::wostringstream threadMes;
	threadMes << "[thread]" << "  \n";
	threadMes << " id: " << std::this_thread::get_id() << "\n";
	OutputW(threadMes.str());

#ifdef _OUTPUT_SXAVENGER_LOG_FILE
	{
		std::unique_lock<std::mutex> lock(mutex_);
		TextW(L"> Exception");
		TextW(locationMes.str());
		TextW(labelMes.str());
		TextW(detailMes.str());
		TextW(threadMes.str());
	}
#endif

	// window
	MessageBoxW(
		NULL,
		(locationMes.str() + L"\n" + labelMes.str()).c_str(),
		L"Sxavenger Engine assertion",
		MB_TASKMODAL | MB_ICONHAND | MB_TOPMOST
	);

	__debugbreak();
}

void SxavengerLogger::LogRuntimeA(Category category, const std::string& label, const std::string& detail) {
	Push({ category, label, detail });
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
	file << mes << "\n";
}

void SxavengerLogger::TextW(const std::wstring& mes) {
	std::wofstream file(filepath_, mode_);
	file << mes << "\n";
}

void SxavengerLogger::Push(const StackData& data) {
	if (stacks_.empty() || stacks_.back().first != data) {
		stacks_.emplace_back(std::make_pair(data, 1));

	} else {
		stacks_.back().second++;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Logger namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

void Logger::Log(const std::string& log) {
	SxavengerLogger::LogA(log);
}

void Logger::Log(const std::wstring& log) {
	SxavengerLogger::LogW(log);
}

void Logger::EngineLog(const std::string& log) {
	std::string tag = "[Sxavenger Engine] >> ";
	SxavengerLogger::LogA(tag + log);
}

void Logger::EngineLog(const std::wstring& log) {
	std::wstring tag = L"[Sxavenger Engine] >> ";
	SxavengerLogger::LogW(tag + log);
}

void Logger::EngineThreadLog(const std::string& log) {
	std::ostringstream tag;
	tag << "[Sxavenger Engine] [thread id: " << std::this_thread::get_id() << "] >> ";
	SxavengerLogger::LogA(tag.str() + log);
}

void Logger::EngineThreadLog(const std::wstring& log) {
	std::wostringstream tag;
	tag << L"[Sxavenger Engine] [thread id: " << std::this_thread::get_id() << L"] >> ";
	SxavengerLogger::LogW(tag.str() + log);
}

void Logger::LogRuntime(const std::string& label, const std::string& detail, SxavengerLogger::Category category) {
	SxavengerLogger::LogRuntimeA(category, label, detail);
}

void Logger::CommentRuntime(const std::string& label, const std::string& detail) {
	SxavengerLogger::LogRuntimeA(SxavengerLogger::Category::Comment, label, detail);
}

void Logger::WarningRuntime(const std::string& label, const std::string& detail) {
	SxavengerLogger::LogRuntimeA(SxavengerLogger::Category::Warning, label, detail);
}

void Logger::ErrorRuntime(const std::string& label, const std::string& detail) {
	SxavengerLogger::LogRuntimeA(SxavengerLogger::Category::Error, label, detail);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Exception namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

void Exception::Assert(bool expression, const std::string& label, const std::string& detail, const std::source_location& location) {
	if (expression) _LIKELY{
		return;
	}

	SxavengerLogger::ExceptionA(label, detail, location);
}

void Exception::AssertW(bool expression, const std::wstring& label, const std::wstring& detail, const std::source_location& location) {
	if (expression) _LIKELY{
		return;
	}

	SxavengerLogger::ExceptionW(label, detail, location);
}
