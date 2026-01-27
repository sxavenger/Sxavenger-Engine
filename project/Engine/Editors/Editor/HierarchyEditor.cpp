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
#include <Engine/Components/Entity/EntityBehaviourStorage.h>
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
		ShowSummaryMenu();

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
			address->SetMobility(EntityBehaviour::Mobility::Static);
		}

		ImGui::Dummy({ 0, 4 });

		ImGui::Text("Punctual Light");
		ImGui::Separator();

		if (ImGui::MenuItem("Directional Light")) {
			BehaviourAddress address = BehaviourHelper::CreateDirectionalLightBehaviour();
			address->SetMobility(EntityBehaviour::Mobility::Static);
		}

		if (ImGui::MenuItem("Point Light")) {
			BehaviourAddress address =  BehaviourHelper::CreatePointLightBehaviour();
			address->SetMobility(EntityBehaviour::Mobility::Static);
		}

		if (ImGui::MenuItem("Spot Light")) {
			BehaviourAddress address = BehaviourHelper::CreateSpotLightBehaviour();
			address->SetMobility(EntityBehaviour::Mobility::Static);
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
				sEntityBehaviourStorage->ClearStaticBehaviours();
				sEntityBehaviourStorage->InputJson(JsonHandler::LoadFromJson(filepath.value()));
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
					sEntityBehaviourStorage->ParseToJson()
				);
			}

			RuntimeLogger::LogComment("[HierarchyEditor]", "save scene.");
		}

		if (ImGui::Button("clear")) {
			sEntityBehaviourStorage->ClearStaticBehaviours();
			RuntimeLogger::LogComment("[HierarchyEditor]", "clear scene.");
		}

		ImGui::EndMenu();
	}
}

void HierarchyEditor::ShowSummaryMenu() {
	if (ImGui::BeginMenu("summary")) {
		MenuPadding();
		ImGui::SeparatorText("summary");


		{ //!< Entity Behaviour Summary
			ImGui::Text("Entity Behaviour Summary");
			ImGui::Separator();

			size_t count        = 0;
			size_t activeCount  = 0;
			size_t rootCount    = 0;
			size_t movableCount = 0;

			sEntityBehaviourStorage->ForEach([&](EntityBehaviour* behaviour) {
				count++;
				activeCount  += behaviour->IsActive();
				rootCount    += behaviour->IsRoot();
				movableCount += (behaviour->GetMobility() == EntityBehaviour::Mobility::Movable);
			});

			ImGui::Text(std::format("behaviour count: {}", count).c_str());
			ImGui::Text(std::format("active behaviour count: {}", activeCount).c_str());
			ImGui::Text(std::format("root behaviour count: {}", rootCount).c_str());
			ImGui::Text(std::format("movable behaviour count: {}", movableCount).c_str());

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
		sEntityBehaviourStorage->ForEachRootOnly([this](EntityBehaviour* behaviour) {
			HierarchySelectable(behaviour);
		});
		
	} else {
		//!< フィルター表示
		sEntityBehaviourStorage->ForEachRootOnly([this](EntityBehaviour* behaviour) {
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
		ImGui::InvisibleButton("## DropTarget Content DragAndDrop", contentSize);

		sContentStorage->DragAndDropTargetContentFunc<ContentModel>([this](const std::shared_ptr<ContentModel>& content) {
			BehaviourAddress address = BehaviourHelper::CreateStaticMeshBehaviour(content);
			address->SetName(content->GetFilepath().stem().generic_string());
			address->SetMobility(EntityBehaviour::Mobility::Static);
		});

		sContentStorage->DragAndDropTargetContentFunc<ContentScene>([this](const std::shared_ptr<ContentScene>& content) {
			content->WaitComplete(); // contentの読み込みを待つ
			sEntityBehaviourStorage->InputJson(content->GetData());
		});

		sContentStorage->DragAndDropTargetContentFunc<ContentBehaviour>([this](const std::shared_ptr<ContentBehaviour>& content) {
			content->WaitComplete(); // contentの読み込みを待つ
			BehaviourAddress address = BehaviourHelper::Create();
			address->InputJson(content->GetData());
			address->SetMobility(EntityBehaviour::Mobility::Static);
		});
	}

	ImGui::End();
}

void HierarchyEditor::ForEachBehaviourHierarchy(const EntityBehaviour::Hierarchy& hierarchy, const std::function<void(EntityBehaviour*)>& function) {
	// child持ちのbehaviour
	for (auto& child : hierarchy | std::views::filter([](const BehaviourAddress& address) { return address->HasChild(); })) {
		function(child.Get());
	}

	// leafのbehaviour
	for (auto& child : hierarchy | std::views::filter([](const BehaviourAddress& address) { return !address->HasChild(); })) {
		function(child.Get());
	}
}

void HierarchyEditor::HierarchySelectable(EntityBehaviour* behaviour) {

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
		ForEachBehaviourHierarchy(behaviour->GetChildren(), [this](EntityBehaviour* child) {
			HierarchySelectable(child);
		});

		ImGui::TreePop();
	}
	
}

bool HierarchyEditor::HierarchyFilter(EntityBehaviour* behaviour, const std::string& filter) {
	// 自身とfilterの比較
	bool result = (behaviour->GetName().find(filter) != std::string::npos);

	// childとfilterの比較
	ForEachBehaviourHierarchy(behaviour->GetChildren(), [&](EntityBehaviour* child) {
		result |= HierarchyFilter(child, filter);
	});

	return result;
}

void HierarchyEditor::HierarchySelectableFilter(EntityBehaviour* behaviour, const std::string& filter) {

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
		ForEachBehaviourHierarchy(behaviour->GetChildren(), [&](EntityBehaviour* child) {
			HierarchySelectableFilter(child, filter);
		});

		ImGui::TreePop();
	}

}

bool HierarchyEditor::CheckSelected(EntityBehaviour* behaviour) {
	if (auto editor = BaseEditor::GetEditorEngine()->GetEditor<InspectorEditor>()) {
		return editor->CheckInspector(behaviour);
	}

	return false;
}

void HierarchyEditor::SetSelected(EntityBehaviour* behaviour) {
	if (auto editor = BaseEditor::GetEditorEngine()->GetEditor<InspectorEditor>()) {
		editor->SetInspector(behaviour);
	}
}

void HierarchyEditor::SetSelectedView(EntityBehaviour* behaviour) {
	if (auto transform = behaviour->GetComponent<TransformComponent>()) {
		if (auto editor = BaseEditor::GetEditorEngine()->GetEditor<RenderSceneEditor>()) {
			editor->SetCameraPoint(transform->GetPosition());
		}
	}
}
