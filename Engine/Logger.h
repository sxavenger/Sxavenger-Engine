#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <string>
#include <format>
#include <fstream>
#include <Windows.h>

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

void AssertMesseage(bool isSuccess, const std::string& errorLog);
void AssertMesseage(bool isSuccess, const std::string& errorLog, const std::string& textTitle);

void AssertLog(bool expression, const std::string& log = "");

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