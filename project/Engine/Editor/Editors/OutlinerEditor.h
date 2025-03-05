#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/Component/MonoBehaviour.h>

//////////////////////////////////////////////////////////////////////////////////////////
// OutlinerEditor class
//////////////////////////////////////////////////////////////////////////////////////////
class OutlinerEditor
	: public BaseEditor {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	OutlinerEditor(EditorEngine* editor) : BaseEditor(editor) {}
	~OutlinerEditor() = default;

	void Init() override;

	void ShowMainMenu() override;

	void ShowWindow() override;

	//* test *//

	void SetBehaviour(MonoBehaviour* behaviour) { behaviour_ = behaviour; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	MonoBehaviour* behaviour_ = nullptr;
	// todo: MonoBehaviourのリストを持つ

	MonoBehaviour* selected_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowOutlinerWindow();
	void ShowInspectorWindow();

	//* sub method *//

	void OutlinerSelectable(MonoBehaviour* behaviour);

};
