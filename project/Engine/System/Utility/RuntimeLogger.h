#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

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
	// Type enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Type : uint8_t {
		Default,
		Comment,
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

		Data(Type _type, const std::string& _category, const std::string& _label);

		void Timestamp();

		bool Compare(const Data& data) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Type type = Type::Comment;

		std::string category;
		std::string label;

		std::chrono::zoned_time<std::chrono::seconds> timestamp;
		size_t count = 0;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* log methods *//

	static void Log(Type type, const std::string& category, const std::string& label);

	static void LogDefault(const std::string& category, const std::string& label);
	static void LogComment(const std::string& category, const std::string& label);
	static void LogWarning(const std::string& category, const std::string& label);
	static void LogError(const std::string& category, const std::string& label);

	//* getter *//

	static const std::list<Data>& GetLogs() { return logs_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline std::list<Data> logs_;

	static inline size_t limit_ = 128;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static void Push(const Data& data);

};

SXAVENGER_ENGINE_NAMESPACE_END
