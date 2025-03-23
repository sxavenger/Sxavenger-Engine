#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/Component/Entity/MonoBehaviour.h>

//* lib
#include <Lib/Geometry/Color4.h>

//////////////////////////////////////////////////////////////////////////////////////////
// HierarchyEditor class
//////////////////////////////////////////////////////////////////////////////////////////
class HierarchyEditor
	: public BaseEditor {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	HierarchyEditor(EditorEngine* editor) : BaseEditor(editor) {}
	~HierarchyEditor() = default;

	void Init() override;

	void ShowMainMenu() override;

	void ShowWindow() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	const Color4f disableColor_ = { 0.4f, 0.4f, 0.4f, 1.0f };

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowHierarchyWindow();

	//* sub method *//

	void HierarchySelectable(MonoBehaviour* behaviour);

	bool CheckSelected(MonoBehaviour* behaviour);

	void SetSelected(MonoBehaviour* behaviour);

};
