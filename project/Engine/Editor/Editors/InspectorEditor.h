#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

////////////////////////////////////////////////////////////////////////////////////////////
// BaseInspector class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseInspector {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	~BaseInspector();

	virtual void ShowInspector() = 0;

	virtual void LateUpdate() {}

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// InspectorEditor class
////////////////////////////////////////////////////////////////////////////////////////////
class InspectorEditor final
	: public BaseEditor {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	InspectorEditor(EditorEngine* editor) : BaseEditor(editor) {}
	~InspectorEditor() = default;

	void Init() override;

	void ShowMainMenu() override;

	void ShowWindow() override;

	void LateUpdate() override;

	//* inspector option *//

	void SetInspector(BaseInspector* inspector) { inspector_ = inspector; }

	void UnregisterInspector() { inspector_ = nullptr; }
	void UnregisterInspector(BaseInspector* target);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* inspector *//

	BaseInspector* inspector_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowInspectorWindow();

};
