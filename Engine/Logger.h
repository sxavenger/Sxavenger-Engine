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
// Output Log
//-----------------------------------------------------------------------------------------

void Log(const std::string& log);
void Log(const std::wstring& logW);

//-----------------------------------------------------------------------------------------
// Assertion
//-----------------------------------------------------------------------------------------

void Assert(bool expresion, const std::string& detail = "", const std::source_location& location = std::source_location::current());
void AssertW(bool expresion, const std::wstring& detail = L"", const std::source_location& location = std::source_location::current());

////////////////////////////////////////////////////////////////////////////////////////////
// ExternalLogger class
////////////////////////////////////////////////////////////////////////////////////////////
class ExternalLogger {
public:

	static void Open();

	static void Close();

	static void Write(const std::string& log);

private:

	static bool isOutput_;

	static const std::string filename_;
	static std::ofstream file_;


};