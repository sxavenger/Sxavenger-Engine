#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <string>
#include <source_location>
#include <filesystem>
#include <fstream>
#include <mutex>

////////////////////////////////////////////////////////////////////////////////////////////
// define
////////////////////////////////////////////////////////////////////////////////////////////

#define _TO_STRING_HELPER(x) #x
#define _TO_STRING(x) _TO_STRING_HELPER(x)

////////////////////////////////////////////////////////////////////////////////////////////
// Convert string methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string  ToString(const std::wstring& str);
std::wstring ToWString(const std::string& str);

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerLogger class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerLogger {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	static void Init();

	//* log option *//

	static void LogA(const std::string& log);
	static void LogW(const std::wstring& log);

	static void ExceptionA(const std::string& label, const std::string& detail, const std::source_location& location);
	static void ExceptionW(const std::wstring& label, const std::wstring& detail, const std::source_location& location);

private:

	//=========================================================================================
	// private variable
	//=========================================================================================

	static const std::filesystem::path filepath_;

	static const std::ofstream::openmode mode_ = std::ofstream::out | std::ofstream::app;

	static std::mutex mutex_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static void OutputA(const std::string& mes);
	static void OutputW(const std::wstring& mes);

	//* write mark down option *//

	static void TextA(const std::string& mes);
	static void TextW(const std::wstring& mes);

};


////////////////////////////////////////////////////////////////////////////////////////////
// Log methods
////////////////////////////////////////////////////////////////////////////////////////////

void Log(const std::string& log);
void Log(const std::wstring& log);

void EngineLog(const std::string& log);
void EngineLog(const std::wstring& log);

void EngineThreadLog(const std::string& log);
void EngineThreadLog(const std::wstring& log);

void Assert(bool expression, const std::string& label = "", const std::string& detail = "", const std::source_location& location = std::source_location::current());
void AssertW(bool expression, const std::wstring& label = L"", const std::wstring& detail = L"", const std::source_location& location = std::source_location::current());
