#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Adapter/Time/LocalTimePoint.h>

//* c++
#include <cstdint>
#include <string>
#include <chrono>
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// RuntimeLogger class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ランタイムログ管理クラス.
class RuntimeLogger final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Level enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Level : uint8_t {
		Information,
		Debug,
		Warning,
		Error,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Data structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Data {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Data(Level _level, const std::string& _category, const std::string& _label);

		void Timestamp();

		bool Compare(const Data& data) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Level level = Level::Information;

		std::string category;
		std::string label;

		LocalTimePoint timestamp;

		size_t count = 0;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* log methods *//

	static void Log(Level level, const std::string& category, const std::string& label);

	static void LogInformation(const std::string& category, const std::string& label) { RuntimeLogger::Log(Level::Information, category, label); }

	static void LogDebug(const std::string& category, const std::string& label) { RuntimeLogger::Log(Level::Debug, category, label); }

	static void LogWarning(const std::string& category, const std::string& label) { RuntimeLogger::Log(Level::Warning, category, label); }

	static void LogError(const std::string& category, const std::string& label) { RuntimeLogger::Log(Level::Error, category, label); }

	//* getter *//

	static const std::list<Data>& GetLogs() { return logs_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline std::list<Data> logs_;

	static inline size_t limit_ = 1 << 10; //!< ログの最大件数

	//=========================================================================================
	// private methods
	//=========================================================================================

	static void Push(const Data& data);

};

SXAVENGER_ENGINE_NAMESPACE_END
