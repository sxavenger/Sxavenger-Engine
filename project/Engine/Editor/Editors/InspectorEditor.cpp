#include "InspectorEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

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

void InspectorEditor::UnregisterInspector(IInspector* target) {
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
