#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <mutex>
#include <thread>
#include <filesystem>
#include <fstream>
#include <string>
#include <source_location>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// StreamLogger class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ストリームログ管理クラス.
class StreamLogger final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	//* logger option *//

	static void Log(const std::string& message);
	static void Log(const std::wstring& message);

	NORETURN static void Exception(const std::string& label, const std::string& detail = "", const std::source_location& location = std::source_location::current());
	NORETURN static void Exception(const std::wstring& label, const std::wstring& detail = L"", const std::source_location& location = std::source_location::current());

	//* engine log option *//

	static void EngineLog(const std::string& message);
	static void EngineLog(const std::wstring& message);

	static void EngineThreadLog(const std::string& message);
	static void EngineThreadLog(const std::wstring& message);

	//* engine assertion option *//

	static void AssertA(bool expression, const std::string& label = "", const std::string& detail = "", const std::source_location& location = std::source_location::current());
	static void AssertW(bool expression, const std::wstring& label = L"", const std::wstring& detail = L"", const std::source_location& location = std::source_location::current());

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ExceptionMessage structure
	////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	struct ExceptionMessage {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		T location;
		T thread;
		T label;
		T detail;

	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* thread *//

	static inline std::mutex mutex_ = {};

	static inline const std::thread::id kMainThreadId_ = std::this_thread::get_id();

	//* log file *//

	static inline const std::filesystem::path kDirectory_ = "Logs";
	static inline const std::ofstream::openmode mode_     = std::ofstream::out | std::ofstream::app;

	static std::filesystem::path filename_;

	static inline bool isInitialized_ = false;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper methods *//

	static std::filesystem::path GetStreamLogFilename();

	//* output methods *//

	static void OutputConsoleA(const std::string& message);
	static void OutputConsoleW(const std::wstring& message);

	static void OutputFileA(const std::string& message);
	static void OutputFileW(const std::wstring& message);

	static void OutputA(const std::string& message);
	static void OutputW(const std::wstring& message);

	static void OutputSeparator();

	//* debug break method *//

	NORETURN static void DebugBreak();

	//* output message helper methods *//

	static std::string GetLocationMessageA(const std::source_location& location);
	static std::wstring GetLocationMessageW(const std::source_location& location);

	static std::string GetThreadMessageA(const std::thread::id id);
	static std::wstring GetThreadMessageW(const std::thread::id id);

	static std::string GetTagMessageA(const std::string& tag, const std::string& text);
	static std::wstring GetTagMessageW(const std::wstring& tag, const std::wstring& text);

	//* output exception helper methods *//

	static ExceptionMessage<std::string> ParseExceptionMessageA(const std::source_location& location, std::thread::id id, const std::string& label, const std::string& detail);
	static ExceptionMessage<std::wstring> ParseExceptionMessageW(const std::source_location& location, std::thread::id id, const std::wstring& label, const std::wstring& detail);

	static void OpenExceptionWindowA(const ExceptionMessage<std::string>& message);
	static void OpenExceptionWindowW(const ExceptionMessage<std::wstring>& message);

};

SXAVENGER_ENGINE_NAMESPACE_END
