#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* utility
#include "UtilityCommon.h"

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <string>
#include <source_location>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <deque>

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerLogger class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerLogger {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Category enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Category {
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

		Category category = Category::None;
		std::string label;
		std::string detail;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using Container = std::deque<std::pair<StackData, size_t>>;

public:

	//=========================================================================================
	// public method
	//=========================================================================================

	static void Init();

	//* log option *//

	static void LogA(const std::string& log);
	static void LogW(const std::wstring& log);

	_NORETURN static void ExceptionA(const std::string& label, const std::string& detail, const std::source_location& location);
	_NORETURN static void ExceptionW(const std::wstring& label, const std::wstring& detail, const std::source_location& location);

	//* runtime log methods *//

	static void LogRuntimeA(Category category, const std::string& label, const std::string& detail = "");

	static const Container& GetStacks() { return stacks_; }

	//* thread check *//

	static bool IsMainThread() { return std::this_thread::get_id() == kMainThreadId_; }
	static bool IsMainThread(const std::thread::id& id) { return id == kMainThreadId_; }

private:

	//=========================================================================================
	// private variable
	//=========================================================================================

	//* thread safety *//

	static std::mutex mutex_;

	//* thread id *//

	static inline const std::thread::id kMainThreadId_ = std::this_thread::get_id();

	//* file output parameter *//

	static const std::filesystem::path filepath_;
	static const std::ofstream::openmode mode_;

	//* log stacks *//

	static inline const size_t kMaxStackSize = 128;
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
// Logger namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Logger {

	void Log(const std::string& log);
	void Log(const std::wstring& log);

	void EngineLog(const std::string& log);
	void EngineLog(const std::wstring& log);

	void EngineThreadLog(const std::string& log);
	void EngineThreadLog(const std::wstring& log);

	void LogRuntime(const std::string& label, const std::string& detail = "", SxavengerLogger::Category category = SxavengerLogger::Category::None);
	void CommentRuntime(const std::string& label, const std::string& detail = "");
	void WarningRuntime(const std::string& label, const std::string& detail = "");
	void ErrorRuntime(const std::string& label, const std::string& detail = "");

}

////////////////////////////////////////////////////////////////////////////////////////////
// Exception namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Exception {

	void Assert(bool expression, const std::string& label = "", const std::string& detail = "", const std::source_location& location = std::source_location::current());
	void AssertW(bool expression, const std::wstring& label = L"", const std::wstring& detail = L"", const std::source_location& location = std::source_location::current());

};



