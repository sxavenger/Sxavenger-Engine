#include "HierarchyEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "../EditorEngine.h"
#include "RenderSceneEditor.h"

//* engine
#include <Engine/Component/MonoBehaviourContainer.h>

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
	ShowInspectorWindow();
}

void HierarchyEditor::CheckResetBehaviour(MonoBehaviour* behaviour) {
	if (behaviour == selected_) {
		selected_ = nullptr;
	}
}

void HierarchyEditor::ShowHierarchyWindow() {
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Hierarchy ## Hierarchy Editor", nullptr, BaseEditor::GetWindowFlag());

	for (auto& behaviour : sMonoBehaviourContainer->GetContainer()) {
		HierarchySelectable(behaviour);
	}

	ImGui::End();
}

void HierarchyEditor::ShowInspectorWindow() {
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Inspector ## Hierarchy Editor", nullptr, BaseEditor::GetWindowFlag());

	if (selected_ != nullptr) { // todo: selected behaviour に変更
		selected_->SetBehaviourImGuiCommand(buf_);

		// Manipulateの設定
		BaseEditor::editor_->ExecuteEditorFunction<RenderSceneEditor>([&](RenderSceneEditor* editor) {
			editor->Manipulate(selected_, ImGuizmo::TRANSLATE, ImGuizmo::WORLD);
		});
	}

	ImGui::End();
}

void HierarchyEditor::HierarchySelectable(MonoBehaviour* behaviour) {

	bool isSelect     = (behaviour == selected_);
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

void HierarchyEditor::SetSelected(MonoBehaviour* behaviour) {
	selected_ = behaviour;
	std::memset(buf_, 0, sizeof(buf_));
	std::memcpy(buf_, behaviour->GetName().c_str(), behaviour->GetName().size());
}
