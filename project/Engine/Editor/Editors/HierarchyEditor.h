#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/Component/MonoBehaviour.h>

//* lib
#include <Lib/Geometry/Color.h>

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

	void LateUpdate() override;

	void CheckResetBehaviour(MonoBehaviour* behaviour);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	MonoBehaviour* selected_ = nullptr;

	char buf_[256] = {};

	//* parameter *//

	const Color4f disableColor_ = { 0.4f, 0.4f, 0.4f, 1.0f };

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
