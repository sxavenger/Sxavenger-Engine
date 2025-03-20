#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/Content/Material/MaterialStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialEditor class
////////////////////////////////////////////////////////////////////////////////////////////
class MaterialEditor
	: public BaseEditor {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	MaterialEditor(EditorEngine* editor) : BaseEditor(editor) {}
	~MaterialEditor() = default;

	void Init() override;

	void ShowMainMenu() override;

	void ShowWindow() override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowMaterialWindow();

};
