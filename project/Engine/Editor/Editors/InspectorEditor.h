#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Interface Inspector class
////////////////////////////////////////////////////////////////////////////////////////////
class IInspector {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	virtual void ShowInspector() = 0;

	virtual void LateUpdate()    = 0;

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

	void SetInspector(IInspector* inspector) { inspector_ = inspector; }

	void UnregisterInspector() { inspector_ = nullptr; }
	void UnregisterInspector(IInspector* target);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* inspector *//

	IInspector* inspector_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowInspectorWindow();

};
