#include "MaterialEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialEditor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MaterialEditor::Init() {
}

void MaterialEditor::ShowMainMenu() {
}

void MaterialEditor::ShowWindow() {
	ShowMaterialWindow();
}

void MaterialEditor::ShowMaterialWindow() {
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Material", nullptr, BaseEditor::GetWindowFlag());
	
	for (const auto& [name, material] : sMaterialStorage->GetStorage()) {
		if (ImGui::TreeNode(name.c_str())) {
			material->SetImGuiCommand();
			ImGui::TreePop();
		}
	}

	ImGui::Separator();

	if (ImGui::Button("output json")) {
		sMaterialStorage->OutputJson();
	}

	ImGui::End();
}
