#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/Component/MonoBehaviour.h>

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

	void CheckResetBehaviour(MonoBehaviour* behaviour);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	MonoBehaviour* selected_ = nullptr;

	char buf_[256] = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowHierarchyWindow();
	void ShowInspectorWindow();

	//* sub method *//

	void HierarchySelectable(MonoBehaviour* behaviour);

	void SetSelected(MonoBehaviour* behaviour);

};
