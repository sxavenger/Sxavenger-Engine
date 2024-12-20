#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Geometry/Vector4.h>

//* external
#include <imgui.h>

//* c++
#include <optional>
#include <string>
#include <deque>
#include <source_location>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Console;

////////////////////////////////////////////////////////////////////////////////////////////
// SystemConsole class
////////////////////////////////////////////////////////////////////////////////////////////
class SystemConsole {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(Console* console);

	void Term();

	void UpdateConsole();

	//* console option *//

	void Log(const std::string& label, const std::string& detail = "", const std::optional<Color4f> color = std::nullopt);

	void BreakPoint(const std::source_location& location = std::source_location::current()); //!< TODO:

	//* getter *//

	bool IsUpdateRequired() const { return isUpdateRequired_; }
	
protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isDisplaySystemConsole_ = true;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* menu *//

	void ShowSystemMenu();

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// LogEntry structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct LogEntry {
		std::string label;            //!< ラベル
		std::string detail;           //!< 詳細
		std::optional<Color4f> color; //!< logの色
		uint32_t duplication = 0;     //!< 重複数
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	Console* console_ = nullptr;

	//* update process *//

	bool isUpdateRequired_ = true; //!< 最終的なプロセス管理

	//! [optional]
	//! nullopt:   通常状態(制限なく更新処理をする)
	//! has_value: 何回更新処理をするか<frame>(0の場合は更新処理を止める)
	std::optional<uint32_t> updateLimit_ = std::nullopt;

	//* log *//

	std::deque<LogEntry> logs_;
	uint32_t limitLog_ = 1 << 4;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* window *//

	void DisplayLog();

	//* assist methods *//

	static void MenuDummy();

};
