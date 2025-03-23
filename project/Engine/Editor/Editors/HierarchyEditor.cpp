#include "HierarchyEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "../EditorEngine.h"
//#include "RenderSceneEditor.h"
#include "InspectorEditor.h"

//* engine
#include <Engine/Component/Entity/MonoBehaviourContainer.h>

//* external
#include <imgui.h>

//////////////////////////////////////////////////////////////////////////////////////////
// HierarchyEditor class methods
//////////////////////////////////////////////////////////////////////////////////////////

void HierarchyEditor::Init() {
}

void HierarchyEditor::ShowMainMenu() {
}

void HierarchyEditor::ShowWindow() {
	ShowHierarchyWindow();
}

void HierarchyEditor::ShowHierarchyWindow() {
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Hierarchy ## Hierarchy Editor", nullptr, BaseEditor::GetWindowFlag());

	for (auto& behaviour : sMonoBehaviourContainer->GetContainer()) {
		HierarchySelectable(behaviour);
	}

	ImGui::End();
}

void HierarchyEditor::HierarchySelectable(MonoBehaviour* behaviour) {

	bool isSelect     = CheckSelected(behaviour);
	std::string label = std::format("{} # {:p}", behaviour->GetName(), reinterpret_cast<const void*>(behaviour));

	if (!behaviour->IsActive()) {
		ImGui::PushStyleColor(ImGuiCol_Text, { disableColor_.r, disableColor_.g, disableColor_.b, disableColor_.a });
	}

	if (behaviour->GetChildren().empty()) {
		if (ImGui::Selectable(label.c_str(), isSelect)) {
			SetSelected(behaviour);
		}

		if (!behaviour->IsActive()) {
			ImGui::PopStyleColor();
		}

	} else {

		ImGuiTreeNodeFlags flags
			= ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_OpenOnArrow;

		if (isSelect) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool isOpen = ImGui::TreeNodeEx(label.c_str(), flags);

		if (!behaviour->IsActive()) {
			ImGui::PopStyleColor();
		}

		if (ImGui::IsItemClicked()) {
			SetSelected(behaviour);
		}

		if (isOpen) {

			for (auto& child : behaviour->GetChildren()) {
				HierarchySelectable(std::visit(MonoBehaviour::GetPtrVisitor{}, child));
			}

			ImGui::TreePop();
		}
	}
}

bool HierarchyEditor::CheckSelected(MonoBehaviour* behaviour) {
	if (auto editor = BaseEditor::GetEditorEngine()->GetEditor<InspectorEditor>()) {
		return editor->CheckInspector(behaviour);
	}

	return false;
}

void HierarchyEditor::SetSelected(MonoBehaviour* behaviour) {
	if (auto editor = BaseEditor::GetEditorEngine()->GetEditor<InspectorEditor>()) {
		editor->SetInspector(behaviour);
	}
}
