#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

////////////////////////////////////////////////////////////////////////////////////////////
// RenderSceneEditor class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderSceneEditor
	: public BaseEditor {
public:
	//=========================================================================================
	// public methods
	//=========================================================================================

	RenderSceneEditor(EditorEngine* editor) : BaseEditor(editor) {}
	~RenderSceneEditor() = default;

	void Init() override;

	void ShowMainMenu() override;

	void ShowWindow() override;

private:


};
