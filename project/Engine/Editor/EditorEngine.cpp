#include "EditorEnigne.h"
#include "EditorEngine.h"

//=========================================================================================
// static variables
//=========================================================================================

const std::string EditorEngine::kEditorName_ = "Sxavenger Engine Editor";

////////////////////////////////////////////////////////////////////////////////////////////
// EditorEngine class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EditorEngine::Init() {

	windowFlag_ = 0;
	windowFlag_ |= ImGuiWindowFlags_NoCollapse;
	windowFlag_ |= ImGuiWindowFlags_NoMove;
	windowFlag_ |= ImGuiWindowFlags_NoResize;
}

void EditorEngine::Term() {
}

void EditorEngine::UpdateEditor() {
#ifdef _DEBUG

	ShowMainMenu();
	ShowWindow();

#endif
}

void EditorEngine::SetNextWindowDocking() const {
	ImGui::SetNextWindowDockID(dockingId_, ImGuiCond_FirstUseEver);
}

EditorEngine* EditorEngine::GetInstance() {
	static EditorEngine instance;
	return &instance;
}

void EditorEngine::ShowMainMenu() {
}

void EditorEngine::ShowWindow() {
	dockingId_ = ImGui::GetID(kEditorName_.c_str());


}

