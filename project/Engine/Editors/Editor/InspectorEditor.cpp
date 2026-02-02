#include "InspectorEditor.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "../EditorEngine.h"

//* engine
#include <Engine/System/UI/SxGui.h>

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

bool BaseInspector::CheckInspector() {

	bool isInspector = false;

	sEditorEngine->ExecuteEditorFunction<InspectorEditor>([&](InspectorEditor* editor) {
		isInspector = editor->CheckInspector(this);
	});

	return isInspector;
}

void BaseInspector::SetInspector() {
	sEditorEngine->ExecuteEditorFunction<InspectorEditor>([this](InspectorEditor* editor) {
		editor->SetInspector(this);
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
		inspector_->LateUpdateInspector();
	}
}

void InspectorEditor::UnregisterInspector(BaseInspector* target) {
	if (target == inspector_) {
		UnregisterInspector();
	}
}

void InspectorEditor::ShowInspectorWindow() {

	std::string label = std::format("{} Inspector ## Inspector Editor", SxGui::Icon::MenuOpen);

	BaseEditor::SetNextWindowDocking();
	ImGui::Begin(label.c_str(), nullptr, BaseEditor::GetWindowFlag());

	if (inspector_) {
		inspector_->ShowInspector();
	}

	ImGui::End();
}
