#pragma once

//-----------------------------------------------------------------------------------------
// Include
//-----------------------------------------------------------------------------------------
#include <windows.h>
#include <string>
#include <format>

//-----------------------------------------------------------------------------------------
// ConertString
//-----------------------------------------------------------------------------------------
std::wstring ConvertString(const std::string& str);

std::string ConvertString(const std::wstring& str);

void Log(const std::string& log);
void Log(const std::wstring& logW);