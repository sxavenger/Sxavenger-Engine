#include "BaseEditor.h"
SXAVENGER_ENGINE_USING

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

bool BaseEditor::IsDisplay() const {
	return editor_->IsEditorDisplay() && isDisplay_;
}

void BaseEditor::MenuPadding() {
	ImGui::Dummy({ kMenuPadding.x, kMenuPadding.y });
}

void BaseEditor::SetNextWindowDocking() {
	editor_->SetNextWindowDocking();
}

uint32_t BaseEditor::GetWindowFlag() const {
	return editor_->GetWindowFlag();
}
