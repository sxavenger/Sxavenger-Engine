#include "InspectorEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "../EditorEngine.h"

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Inspector class methods
////////////////////////////////////////////////////////////////////////////////////////////

BaseInspector::~BaseInspector() {
	sEditorEngine->ExecuteEditorFunction<InspectorEditor>([this](InspectorEditor* editor) {
		editor->UnregisterInspector(this);
	});
}

////////////////////////////////////////////////////////////////////////////////////////////
// InspectorEditor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void InspectorEditor::Init() {
}

void InspectorEditor::ShowMainMenu() {
}

void InspectorEditor::ShowWindow() {
	ShowInspectorWindow();
}

void InspectorEditor::LateUpdate() {
	if (inspector_) {
		inspector_->LateUpdate();
	}
}

void InspectorEditor::UnregisterInspector(BaseInspector* target) {
	if (target == inspector_) {
		UnregisterInspector();
	}
}

void InspectorEditor::ShowInspectorWindow() {
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Inspector ## Engine Inspector Editor", nullptr, BaseEditor::GetWindowFlag());

	if (inspector_) {
		inspector_->ShowInspector();
	}

	ImGui::End();
}
