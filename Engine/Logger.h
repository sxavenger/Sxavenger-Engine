#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

#include <string>
#include <format>
#include <fstream>

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

void Assert(bool isSuccess, const std::string& errorLog);
void Assert(bool isSuccess, const std::string& errorLog, const std::string& textTitle);

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