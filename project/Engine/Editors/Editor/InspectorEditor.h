#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/Foundation.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BaseInspector class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Inspector表示用基底クラス
class BaseInspector {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	~BaseInspector();

	virtual void ShowInspector() = 0;

	virtual void LateUpdateInspector() {}

	//* inspector option *//

	bool CheckInspector();

	void SetInspector();

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// InspectorEditor class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Inspector用Editorクラス
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

	bool CheckInspector(BaseInspector* target) const { return inspector_ == target; }

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

SXAVENGER_ENGINE_NAMESPACE_END
