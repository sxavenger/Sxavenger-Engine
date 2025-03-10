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
#include <vector>

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

	////////////////////////////////////////////////////////////////////////////////////////////
	// Status enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Status {
		None,
		Comment,
		Warning,
		Error,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// StackData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct StackData {
	public:

		//=========================================================================================
		// operator
		//=========================================================================================

		bool operator==(const StackData& data) const {
			return label == data.label && detail == data.detail;
		}

		bool operator!=(const StackData& data) const {
			return !(*this == data);
		}

		//=========================================================================================
		// public variables
		//=========================================================================================

		Status status = Status::None;
		std::string label;
		std::string detail;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using Container = std::vector<std::pair<StackData, size_t>>;

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

	//* runtime log methods *//

	static void LogRuntimeA(Status status, const std::string& label, const std::string& detail = "");

	static const Container& GetStacks() { return stacks_; }

private:

	//=========================================================================================
	// private variable
	//=========================================================================================

	//* thread safety *//

	static std::mutex mutex_;

	//* file output parameter *//

	static const std::filesystem::path filepath_;
	static const std::ofstream::openmode mode_;

	//* log stacks *//

	static Container stacks_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* output debug log *//

	static void OutputA(const std::string& mes);
	static void OutputW(const std::wstring& mes);

	//* output file text *//

	static void TextA(const std::string& mes);
	static void TextW(const std::wstring& mes);

	//* stack methods *//

	static void Push(const StackData& data);

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

void LogRuntime(const std::string& label, const std::string& detail = "", SxavengerLogger::Status status = SxavengerLogger::Status::None);
void CommentRuntime(const std::string& label, const std::string& detail = "");
void WarningRuntime(const std::string& label, const std::string& detail = "");
void ErrorRuntime(const std::string& label, const std::string& detail = "");
