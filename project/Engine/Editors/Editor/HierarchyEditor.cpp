#include "HierarchyEditor.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "../EditorEngine.h"
#include "InspectorEditor.h"
#include "RenderSceneEditor.h"

//* engine
#include <Engine/System/Utility/RuntimeLogger.h>
#include <Engine/System/UI/SxImGui.h>
#include <Engine/Assets/Content/ContentStorage.h>
#include <Engine/Components/Entity/MonoBehaviourStorage.h>
#include <Engine/Components/Entity/BehaviourHelper.h>

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

void HierarchyEditor::LateUpdate() {
}

void HierarchyEditor::ShowHierarchyMenu() {
	if (ImGui::BeginMenu("hierarchy")) {
		MenuPadding();
		ImGui::SeparatorText("hierarchy");

		ShowActorMenu();
		ShowSceneMenu();

		ImGui::EndMenu();
	}
}

void HierarchyEditor::ShowActorMenu() {
	if (ImGui::BeginMenu("actor")) {
		MenuPadding();
		ImGui::SeparatorText("actor");

		ImGui::Text("Common");
		ImGui::Separator();

		if (ImGui::MenuItem("Behaviour")) {
			BehaviourAddress address = BehaviourHelper::CreateTransformBehaviour();
			address->SetMobility(MonoBehaviour::Mobility::Static);
		}

		ImGui::Dummy({ 0, 4 });

		ImGui::Text("Punctual Light");
		ImGui::Separator();

		if (ImGui::MenuItem("Directional Light")) {
			BehaviourAddress address = BehaviourHelper::CreateDirectionalLightBehaviour();
			address->SetMobility(MonoBehaviour::Mobility::Static);
		}

		if (ImGui::MenuItem("Point Light")) {
			BehaviourAddress address =  BehaviourHelper::CreatePointLightBehaviour();
			address->SetMobility(MonoBehaviour::Mobility::Static);
		}

		if (ImGui::MenuItem("Spot Light")) {
			BehaviourAddress address = BehaviourHelper::CreateSpotLightBehaviour();
			address->SetMobility(MonoBehaviour::Mobility::Static);
		}

		ImGui::EndMenu();
	}
}

void HierarchyEditor::ShowSceneMenu() {
	if (ImGui::BeginMenu("scene")) {
		MenuPadding();
		ImGui::SeparatorText("scene");

		if (ImGui::Button("load")) {

			std::optional<std::filesystem::path> filepath = WinApp::GetOpenFilepath(
				L"Sceneを選択",
				kSceneDirectory,
				{ L"Scene File", L"*.scene" }
			);

			if (filepath.has_value()) {
				sMonoBehaviourStorage->ClearStaticBehaviours();
				sMonoBehaviourStorage->InputJson(JsonHandler::LoadFromJson(filepath.value()));
			}

			RuntimeLogger::LogComment("[HierarchyEditor]", "load scene.");
		}

		ImGui::SameLine();

		if (ImGui::Button("save")) {

			std::optional<std::filesystem::path> filepath = WinApp::GetSaveFilepath(
				L"Sceneを保存",
				kSceneDirectory,
				{ L"Scene File", L"*.scene" },
				".scene"
			);

			if (filepath.has_value()) {
				JsonHandler::WriteToJson(
					filepath.value(),
					sMonoBehaviourStorage->PerseToJson()
				);
			}

			RuntimeLogger::LogComment("[HierarchyEditor]", "save scene.");
		}

		if (ImGui::Button("clear")) {
			sMonoBehaviourStorage->ClearStaticBehaviours();
			RuntimeLogger::LogComment("[HierarchyEditor]", "clear scene.");
		}

		ImGui::EndMenu();
	}
}

void HierarchyEditor::ShowHierarchyWindow() {
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Hierarchy ## Hierarchy Editor", nullptr, BaseEditor::GetWindowFlag());

	SxImGui::InputText("## hierarchy filter", hierarchyBuf_);

	ImGui::Separator();

	// hierarchyの表示
	ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0);

	if (hierarchyBuf_.empty()) {
		//!< 通常表示
		sMonoBehaviourStorage->ForEachRootOnly([this](MonoBehaviour* behaviour) {
			HierarchySelectable(behaviour);
		});
		
	} else {
		//!< フィルター表示
		sMonoBehaviourStorage->ForEachRootOnly([this](MonoBehaviour* behaviour) {
			HierarchySelectableFilter(behaviour, hierarchyBuf_);
		});
	}

	ImGui::PopStyleVar();

	{
		ImVec2 position = ImGui::GetWindowPos();
		ImVec2 size     = ImGui::GetWindowSize();

		ImVec2 min = ImGui::GetWindowContentRegionMin();
		ImVec2 max = ImGui::GetWindowContentRegionMax();

		ImVec2 contentPos  = ImVec2(position.x + min.x, position.y + min.y);
		ImVec2 contentSize = ImVec2(max.x - min.x, max.y - min.y);

		// InvisibleButton をウィンドウ内部の描画領域全体に敷く

		//* UContentModel
		ImGui::SetCursorScreenPos(contentPos);
		ImGui::InvisibleButton("## DropTarget ContentModel", contentSize);

		sContentStorage->DragAndDropTargetContentFunc<ContentModel>([this](const std::shared_ptr<ContentModel>& content) {
			BehaviourAddress address = BehaviourHelper::CreateStaticMeshBehaviour(content);
			address->SetName(content->GetFilepath().stem().generic_string());
			address->SetMobility(MonoBehaviour::Mobility::Static);
		});

		ImGui::SetCursorScreenPos(contentPos);
		ImGui::InvisibleButton("## DropTarget Scene", contentSize);

		sContentStorage->DragAndDropTargetContentFunc<ContentScene>([this](const std::shared_ptr<ContentScene>& content) {
			content->WaitComplete(); // contentの読み込みを待つ
			sMonoBehaviourStorage->InputJson(content->GetData());
		});
	}

	ImGui::End();
}

void HierarchyEditor::ForEachBehaviourHierarchy(const MonoBehaviour::Hierarchy& hierarchy, const std::function<void(MonoBehaviour*)>& function) {
	// child持ちのbehaviour
	for (auto& child : hierarchy | std::views::filter([](const BehaviourAddress& address) { return address->HasChild(); })) {
		function(child.Get());
	}

	// leafのbehaviour
	for (auto& child : hierarchy | std::views::filter([](const BehaviourAddress& address) { return !address->HasChild(); })) {
		function(child.Get());
	}
}

void HierarchyEditor::HierarchySelectable(MonoBehaviour* behaviour) {

	bool isSelect     = CheckSelected(behaviour);
	std::string label = std::format("{} # {:p}", behaviour->GetName(), static_cast<const void*>(behaviour));

	bool hasChild = behaviour->HasChild();

	if (!behaviour->IsActive()) {
		ImGui::PushStyleColor(ImGuiCol_Text, { disableColor_.r, disableColor_.g, disableColor_.b, disableColor_.a });
	}

	ImGuiTreeNodeFlags flags
		= ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_FramePadding
		| ImGuiTreeNodeFlags_SpanAllColumns
		| ImGuiTreeNodeFlags_DrawLinesToNodes;

	if (isSelect) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	if (!hasChild) {
		flags |= ImGuiTreeNodeFlags_Leaf;
		ImGui::Unindent();
	}

	bool isOpen = ImGui::TreeNodeEx(label.c_str(), flags);

	if (!hasChild) {
		ImGui::Indent();
	}

	if (!behaviour->IsActive()) {
		ImGui::PopStyleColor();
	}

	//* event

	if (ImGui::IsItemClicked()) {
		SetSelected(behaviour);
		isSelect = true;
	}

	if (isSelect && SxImGui::IsDoubleClickItem()) {
		SetSelectedView(behaviour);
	}

	if (ImGui::BeginPopupContextItem(std::format("hierarchy behaviour context menu # {:p}", static_cast<const void*>(behaviour)).c_str())) {
		ImGui::PopStyleVar();

		ImGui::SeparatorText("behaviour context menu");
		ImGui::Text("name: %s", behaviour->GetName().c_str());

		ImGui::Separator();

		if (ImGui::MenuItem("Add Child")) {
			behaviour->AddChild(BehaviourHelper::CreateTransformBehaviour());
		}

		// TODO: Removeの追加

		ImGui::EndPopup();

		ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0);
	}

	if (isOpen) {
		ForEachBehaviourHierarchy(behaviour->GetChildren(), [this](MonoBehaviour* child) {
			HierarchySelectable(child);
		});

		ImGui::TreePop();
	}
	
}

bool HierarchyEditor::HierarchyFilter(MonoBehaviour* behaviour, const std::string& filter) {
	// 自身とfilterの比較
	bool result = (behaviour->GetName().find(filter) != std::string::npos);

	// childとfilterの比較
	ForEachBehaviourHierarchy(behaviour->GetChildren(), [&](MonoBehaviour* child) {
		result |= HierarchyFilter(child, filter);
	});

	return result;
}

void HierarchyEditor::HierarchySelectableFilter(MonoBehaviour* behaviour, const std::string& filter) {

	if (!HierarchyFilter(behaviour, filter)) {
		return;
	}

	bool isSelect     = CheckSelected(behaviour);
	std::string label = std::format("{} # {:p}", behaviour->GetName(), static_cast<const void*>(behaviour));

	bool hasChild = !behaviour->HasChild();

	if (!behaviour->IsActive()) {
		ImGui::PushStyleColor(ImGuiCol_Text, { disableColor_.r, disableColor_.g, disableColor_.b, disableColor_.a });
	}

	ImGuiTreeNodeFlags flags
		= ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_FramePadding
		| ImGuiTreeNodeFlags_SpanAllColumns
		| ImGuiTreeNodeFlags_DrawLinesToNodes;

	if (isSelect) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	if (!hasChild) {
		flags |= ImGuiTreeNodeFlags_Leaf;
		ImGui::Unindent();
	}

	bool isOpen = ImGui::TreeNodeEx(label.c_str(), flags);

	if (!hasChild) {
		ImGui::Indent();
	}

	if (!behaviour->IsActive()) {
		ImGui::PopStyleColor();
	}

	//* event

	if (ImGui::IsItemClicked()) {
		SetSelected(behaviour);
		isSelect = true;
	}

	if (isSelect && SxImGui::IsDoubleClickItem()) {
		SetSelectedView(behaviour);
	}

	if (ImGui::BeginPopupContextItem(std::format("hierarchy behaviour context menu # {:p}", static_cast<const void*>(behaviour)).c_str())) {
		ImGui::PopStyleVar();

		ImGui::SeparatorText("behaviour context menu");
		ImGui::Text("name: %s", behaviour->GetName().c_str());

		ImGui::Separator();

		if (ImGui::MenuItem("Add Child")) {
			behaviour->AddChild(BehaviourHelper::CreateTransformBehaviour());
		}

		// TODO: Removeの追加

		ImGui::EndPopup();

		ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0);
	}

	if (isOpen) {
		ForEachBehaviourHierarchy(behaviour->GetChildren(), [&](MonoBehaviour* child) {
			HierarchySelectableFilter(child, filter);
		});

		ImGui::TreePop();
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

void HierarchyEditor::SetSelectedView(MonoBehaviour* behaviour) {
	if (auto transform = behaviour->GetComponent<TransformComponent>()) {
		if (auto editor = BaseEditor::GetEditorEngine()->GetEditor<RenderSceneEditor>()) {
			editor->SetCameraPoint(transform->GetPosition());
		}
	}
}
