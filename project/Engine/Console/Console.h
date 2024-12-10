#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* console
#include "SystemConsole/SystemConsole.h"
#include "RenderConsole/RenderConsole.h"
#include "AssetConsole/AssetConsole.h"

//* engine
#include <Engine/System/UI/ImGuiController.h>
#include <Engine/System/Runtime/Performance/Performance.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/System/Window/GameWindow.h>

//* external
#include <imgui.h>

//* c++
#include <string>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Console class
////////////////////////////////////////////////////////////////////////////////////////////
class Console
	: public SystemConsole, public RenderConsole, public AssetConsole {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void UpdateConsole();

	void Draw();

	void DrawConsole();

	//* console docking *//

	void DockingConsole() const;

	//* getter *//

	ImGuiWindowFlags GetWindowFlag() const { return windowFlag_; } //!< HACK:

	//* setter *//

	void SetWindow(const std::weak_ptr<GameWindow>& window);

	//* singleton *//

	static Console* GetInstance();
	Console()  = default;
	~Console() = default;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	std::weak_ptr<GameWindow> window_;
	std::weak_ptr<GameWindow> subWindow_;

	//* config *//
	//* console
	bool isDisplayConsole_ = true;

	//* imgui
	ImGuiID dockingId_           = NULL;
	ImGuiWindowFlags windowFlag_ = NULL;

	//* parameter *//

	static const std::string kConsoleName_;

	//* runtime *//

	RunTimeTracker runtime_;

	std::deque<float> fpsHistory_;
	static const uint32_t kHistoryCount_ = 60 * 4;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* menu *//

	void DisplayMainMenu();

	//* window *//

	void DisplayPerformace();

	//* sub methods *//

	bool IsForcusConsoleWindow();

	void BeginDisabled(bool isDisable = true);
	void EndDisabled();

	static void MenuDummy();

};

////////////////////////////////////////////////////////////////////////////////////////////
// Console instance
////////////////////////////////////////////////////////////////////////////////////////////
static Console* const sConsole = Console::GetInstance();