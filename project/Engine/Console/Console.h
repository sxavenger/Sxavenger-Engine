#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* console
#include "SystemConsole.h"

//* engine
#include <Engine/System/UI/ImGuiController.h>
#include <Engine/System/Runtime/Performance/Performance.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>

//* external
#include <imgui.h>

//* c++
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Console class
////////////////////////////////////////////////////////////////////////////////////////////
class Console
	: public SystemConsole {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void UpdateConsole();

	void Draw();

	//* console docking *//

	void DockingConsole() const;

	//* getter *//

	ImGuiWindowFlags GetWindowFlag() const { return windowFlag_; } //!< HACK:

	//* singleton *//

	static Console* GetInstance();
	Console()  = default;
	~Console() = default;

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

	//* parameter *//

	static const std::string kConsoleName_;
	static const std::string kImGuiLayoutFilepath_;

	//* runtime *//

	RunTimeTracker runtime_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* menu *//

	void DisplayMainMenu();

	//* window *//

	void DisplayPerformace();

};

////////////////////////////////////////////////////////////////////////////////////////////
// Console instance
////////////////////////////////////////////////////////////////////////////////////////////
static Console* const sConsole = Console::GetInstance();