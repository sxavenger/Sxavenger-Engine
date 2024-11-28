#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine

//* lib
#include <Lib/Geometry/Vector4.h>

//* external
#include <imgui.h>

//* c++
#include <optional>
#include <string>
#include <deque>

////////////////////////////////////////////////////////////////////////////////////////////
// SystemConsole class
////////////////////////////////////////////////////////////////////////////////////////////
class SystemConsole {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void UpdateConsole();

	//* console option *//

	void Log(const std::string& log, const std::optional<Color4f>& color = std::nullopt);

	void BreakPoint(); //!< TODO:

	//* getter *//

	bool IsUpdateRequired() const { return isUpdateRequired_; }
	
protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isDisplaySystemConsole_ = true;

	//* update process *//

	bool isUpdateRequired_ = true; //!< 最終的なプロセス管理

	//! [optional]
	//! nullopt:   通常状態(制限なく更新処理をする)
	//! has_value: 何回更新処理をするか<frame>(0の場合は更新処理を止める)
	std::optional<uint32_t> updateLimit_ = std::nullopt;

	//* log *//

	//! [pair]
	//! first: ログ内容
	//!  second: [optional]
	//!  nullopt: 通常text
	//!  has_value:    colorText
	std::deque<std::pair<std::string, std::optional<Color4f>>> logs_;
	uint32_t limitLog_ = 32;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* menu *//

	void DisplaySystemMenu();

	//* window *//

	void DisplayLog();

	//* assist methods *//

	static void MenuDummy();

};