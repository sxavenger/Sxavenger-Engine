#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <string>
#include <format>
#include <fstream>
#include <source_location>

//-----------------------------------------------------------------------------------------
// Convert to string || wstring
//-----------------------------------------------------------------------------------------

std::wstring ToWstring(const std::string& str);
std::string ToString(const std::wstring& str);

//-----------------------------------------------------------------------------------------
// Log
//-----------------------------------------------------------------------------------------

void Log(const std::string& log);
void Log(const std::wstring& logW);

//-----------------------------------------------------------------------------------------
// Assertion
//-----------------------------------------------------------------------------------------

void Assert(bool expresion, const std::string& detail = "", const std::source_location& location = std::source_location::current());
void AssertW(bool expresion, const std::wstring& detail = L"", const std::source_location& location = std::source_location::current());

//-----------------------------------------------------------------------------------------
// Output
//-----------------------------------------------------------------------------------------

void OutputLog(const std::string& log, const std::string& filepath);

////////////////////////////////////////////////////////////////////////////////////////////
// ExternalLogger class
////////////////////////////////////////////////////////////////////////////////////////////
class ExternalLogger {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Open();

	static void Close();

	static void Write(const std::string& log);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================


	static const std::string filename_;
	static std::ofstream file_;

	static bool isOutput_;

};