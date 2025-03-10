#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* c++
#include <optional>

//////////////////////////////////////////////////////////////////////////////////////////
// DevelopEditor class
//////////////////////////////////////////////////////////////////////////////////////////
class DevelopEditor
	: public BaseEditor{
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	DevelopEditor(EditorEngine* editor) : BaseEditor(editor) {}
	~DevelopEditor() = default;

	void Init() override;

	void ShowMainMenu() override;

	void ShowWindow() override;

	//* getter *//

	bool IsProcessRequired() const { return isProcessRequired_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* update state *//

	bool isProcessRequired_ = true; //!< 最終的なプロセス管理

	//! [optional]
	//! nullopt:   通常状態(制限なく更新処理をする)
	//! has_value: 何回更新処理をするか<frame>(0の場合は更新処理を止める)
	std::optional<uint32_t> processLimit_ = std::nullopt;

	//* console option *//

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowProcessMenu();
	void ShowSystemMenu();
	void ShowThreadMenu();

	void ShowPerformanceWindow();
	void ShowConsole();

	//* sub method *//

	void BreakPoint();

};
