#include "HierarchyEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "../EditorEngine.h"
#include "InspectorEditor.h"

//* engine
#include <Engine/Asset/AssetStorage.h>
#include <Engine/Component/Entity/MonoBehaviourContainer.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/Module/Scene/SceneObjects.h>

//* external
#include <imgui.h>

//////////////////////////////////////////////////////////////////////////////////////////
// HierarchyEditor class methods
//////////////////////////////////////////////////////////////////////////////////////////

void HierarchyEditor::Init() {
}

void HierarchyEditor::ShowMainMenu() {
	ShowHierarchyMenu();
}

void HierarchyEditor::ShowWindow() {
	ShowHierarchyWindow();
}

void HierarchyEditor::ShowHierarchyMenu() {
	if (ImGui::BeginMenu("hierarchy")) {
		MenuPadding();
		ImGui::SeparatorText("hierarchy");

		if (ImGui::Button("output")) {
			sSceneObjects->OutputJson();
			CommentRuntime("success | output scene");
		}

		ImGui::SameLine();

		if (ImGui::Button("load")) {
			sSceneObjects->InputJson();
			CommentRuntime("success | load scene");
		}

		ImGui::EndMenu();
	}
}

void HierarchyEditor::ShowHierarchyWindow() {
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Hierarchy ## Hierarchy Editor", nullptr, BaseEditor::GetWindowFlag());

	// hierarchyの表示
	for (auto& behaviour : sMonoBehaviourContainer->GetContainer()) {
		HierarchySelectable(behaviour);
	}

	{
		ImVec2 position = ImGui::GetWindowPos();
		ImVec2 size = ImGui::GetWindowSize();

		ImVec2 min = ImGui::GetWindowContentRegionMin();
		ImVec2 max = ImGui::GetWindowContentRegionMax();

		ImVec2 contentPos = ImVec2(position.x + min.x, position.y + min.y);
		ImVec2 contentSize = ImVec2(max.x - min.x, max.y - min.y);

		// InvisibleButton をウィンドウ内部の描画領域全体に敷く
		ImGui::SetCursorScreenPos(contentPos);
		ImGui::InvisibleButton("##DropTarget", contentSize);

		DragAndDropTarget();
	}

	ImGui::End();
}

void HierarchyEditor::LateUpdate() {
}

void HierarchyEditor::HierarchySelectable(MonoBehaviour* behaviour) {

	bool isSelect     = CheckSelected(behaviour);
	std::string label = std::format("{} # {:p}", behaviour->GetName(), static_cast<const void*>(behaviour));

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

void HierarchyEditor::DragAndDropTarget() {
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(typeid(AssetModel).name())) {
			const char* filepath = static_cast<const char*>(payload->Data);

			std::unique_ptr<SceneObject> object = std::make_unique<SceneObject>();
			object->CreateMeshComponent(filepath);
			sSceneObjects->AddObject(std::move(object));
		}

		ImGui::EndDragDropTarget();
	}
}
