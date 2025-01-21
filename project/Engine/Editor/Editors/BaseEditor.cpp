#include "BaseEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "../EditorEngine.h"

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Base Editor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseEditor::MenuPadding() {
	ImGui::Dummy({ 240.0f, 0.0f });
}

void BaseEditor::SetNextWindowDocking() {
	editor_->SetNextWindowDocking();
}

uint32_t BaseEditor::GetWindowFlag() const {
	return editor_->GetWindowFlag();
}
