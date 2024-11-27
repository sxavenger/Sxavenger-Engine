#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <string>
#include <source_location>

////////////////////////////////////////////////////////////////////////////////////////////
// Convert string methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string  ToString(const std::wstring& str);
std::wstring ToWString(const std::string& str);

////////////////////////////////////////////////////////////////////////////////////////////
// Log methods
////////////////////////////////////////////////////////////////////////////////////////////

void Log(const std::string& log);
void Log(const std::wstring& log);

void EngineLog(const std::string& log);
void EngineLog(const std::wstring& log);

void ThreadLog(const std::string& log);
void ThreadLog(const std::wstring& log);

////////////////////////////////////////////////////////////////////////////////////////////
// Assert methods
////////////////////////////////////////////////////////////////////////////////////////////

void Assert(bool expresion, const std::string& label = "", const std::string& detail = "", const std::source_location& location = std::source_location::current());
void AssertW(bool expresion, const std::wstring& label = L"", const std::wstring& detail = L"", const std::source_location& location = std::source_location::current());