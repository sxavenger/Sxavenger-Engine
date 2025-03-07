#include "OutlinerEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "../EditorEngine.h"
#include "RenderSceneEditor.h"

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

	if (behaviour_ != nullptr) {
		OutlinerSelectable(behaviour_);
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
	if (behaviour->GetChildren().empty()) {
		if (ImGui::Selectable(behaviour->GetName().c_str())) {
			// todo: 選択処理
			selected_ = behaviour;
		}

	} else {

		if (ImGui::TreeNode(behaviour->GetName().c_str())) {
			// todo: 選択処理

			for (auto& child : behaviour->GetChildren()) {
				OutlinerSelectable(std::visit(MonoBehaviour::GetPtrVisitor{}, child));
			}

			ImGui::TreePop();
		}
	}
}
