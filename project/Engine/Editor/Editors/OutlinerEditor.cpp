#include "OutlinerEditor.h"

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
// OutlinerEditor class methods
//////////////////////////////////////////////////////////////////////////////////////////

void OutlinerEditor::Init() {
}

void OutlinerEditor::ShowMainMenu() {
}

void OutlinerEditor::ShowWindow() {
	ShowOutlinerWindow();
	ShowInspectorWindow();
}

void OutlinerEditor::ShowOutlinerWindow() {
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Outliner ## Outliner Editor", nullptr, BaseEditor::GetWindowFlag());

	for (auto& behaviour : sMonoBehaviourContainer->GetContainer()) {
		OutlinerSelectable(behaviour);
	}

	ImGui::End();
}

void OutlinerEditor::ShowInspectorWindow() {
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Inspector ## Outliner Editor", nullptr, BaseEditor::GetWindowFlag());

	if (selected_ != nullptr) { // todo: selected behaviour に変更
		for (const auto& [type, component] : selected_->GetComponents()) {
			if (ImGui::CollapsingHeader(type.name())) {
				(*component)->InspectorImGui();
			}
		}

		// Manipulateの設定
		BaseEditor::editor_->ExecuteEditorFunction<RenderSceneEditor>([&](RenderSceneEditor* editor) {
			editor->Manipulate(selected_, ImGuizmo::TRANSLATE, ImGuizmo::WORLD);
		});
	}

	ImGui::End();
}

void OutlinerEditor::OutlinerSelectable(MonoBehaviour* behaviour) {

	bool isSelect     = (behaviour == selected_);
	std::string label = std::format("{} # {:p}", behaviour->GetName(), reinterpret_cast<const void*>(behaviour));

	if (behaviour->GetChildren().empty()) {
		if (ImGui::Selectable(label.c_str(), isSelect)) {
			// todo: 選択処理
			selected_ = behaviour;
		}

	} else {

		ImGuiTreeNodeFlags flags
			= ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_OpenOnArrow;

		if (isSelect) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool isOpen = ImGui::TreeNodeEx(label.c_str(), flags);

		if (ImGui::IsItemClicked()) {
			selected_ = behaviour;
			//localCamera_->Reset();
		}

		if (isOpen) {

			for (auto& child : behaviour->GetChildren()) {
				OutlinerSelectable(std::visit(MonoBehaviour::GetPtrVisitor{}, child));
			}

			ImGui::TreePop();
		}
	}
}
