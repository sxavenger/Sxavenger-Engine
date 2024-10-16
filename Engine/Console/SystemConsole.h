#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

//* lib
#include <Lib/Geometry/Vector4.h>

//* c++
#include <optional>
#include <string>
#include <deque>

////////////////////////////////////////////////////////////////////////////////////////////
// SystemConsole class
////////////////////////////////////////////////////////////////////////////////////////////
class SystemConsole final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void UpdateConsole();

	//* console docking *//

	void DockingConsole() const;

	//* console option *//

	void Log(const std::string& log, const std::optional<Color4f>& color = std::nullopt);

	//* config getter *//

	const ImGuiWindowFlags GetWindowFlag() const { return windowFlag_; }

	//* other console getter *//

		

	//!< singleton
	// TODO: SxavengerEngineの関数として入れてもいいかも
	static SystemConsole* const GetInstance();
	SystemConsole()  = default;
	~SystemConsole() = default;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* config *//
	//* console
	bool isDisplayConsole_ = true;

	//* imgui
	ImGuiID dockingId_           = NULL;
	ImGuiWindowFlags windowFlag_ = NULL;

	//* update process *//

	bool isUpdateRequired_ = true; //!< 最終的なプロセス管理

	//! [optional]
	//! nullopt:   通常状態(制限なく更新処理をする)
	//! has_value: 何回更新処理をするか<frame>(0の場合は更新処理を止める)
	std::optional<uint32_t> updateLimit_ = std::nullopt;

	//* parameter *//

	static const std::string kConsoleName_;
	static const std::string kImGuiIniFilepath_;

	//* log *//

	//! [pair]
	//! first: ログ内容
	//!  second: [optional]
	//!  std::nullopt: 通常text
	//!  has_value:    colorText
	std::deque<std::pair<std::string, std::optional<Color4f>>> logs_;
	uint32_t limitLog_ = 32;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitImGuiConfig();

	//* display console methods *//

	void DisplayMainMenu();
	void DisplayPerformance();
	void DisplayLog();

	void DisplaySystemMenu();
};

SystemConsole* const sSystemConsole = SystemConsole::GetInstance();