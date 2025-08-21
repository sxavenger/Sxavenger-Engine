#include "HierarchyEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "../EditorEngine.h"
#include "InspectorEditor.h"
#include "RenderSceneEditor.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
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

	if (ImGui::BeginMenu("hierarchy")) {
		MenuPadding();
		ImGui::SeparatorText("hierarchy");

		ImGui::EndMenu();
	}
}

void HierarchyEditor::ShowWindow() {
	ShowHierarchyWindow();
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
			std::unique_ptr<SceneObject> object = std::make_unique<SceneObject>();
			object->SetName("behaviour");

			ComponentHelper::CreateTransformMonoBehaviour(object.get());

			sSceneObjects->AddObject(std::move(object));
		}

		ImGui::Dummy({ 0, 4 });

		ImGui::Text("Punctual Light");
		ImGui::Separator();

		if (ImGui::MenuItem("Directional Light")) {
			std::unique_ptr<SceneObject> object = std::make_unique<SceneObject>();
			object->SetName("Directional Light");

			ComponentHelper::CreateDirectionalLightMonoBehaviour(object.get());

			sSceneObjects->AddObject(std::move(object));
		}

		if (ImGui::MenuItem("Point Light")) {
			std::unique_ptr<SceneObject> object = std::make_unique<SceneObject>();
			object->SetName("Point Light");

			ComponentHelper::CreatePointLightMonoBehaviour(object.get());

			sSceneObjects->AddObject(std::move(object));
		}

		ImGui::EndMenu();
	}
}

void HierarchyEditor::ShowSceneMenu() {
	if (ImGui::BeginMenu("scene")) {
		MenuPadding();
		ImGui::SeparatorText("scene");

		SxImGui::InputTextFunc("## scene name", sceneNameBuf_, [this](const std::string& buf) {
			sceneFileName_ = buf;
		});

		if (ImGui::Button("load")) {

			if (sceneFileName_.extension() != ".scene") {
				sceneFileName_ = sceneFileName_.replace_extension(".scene");
			}

			sSceneObjects->InputJsonFromFilepath(kSceneDirectory_ / sceneFileName_);
			Logger::CommentRuntime("success | load scene");
		}

		ImGui::SameLine();

		if (ImGui::Button("save")) {

			if (sceneFileName_.extension() != ".scene") {
				sceneFileName_ = sceneFileName_.replace_extension(".scene");
			}

			sSceneObjects->OutputJson(kSceneDirectory_ / sceneFileName_);
			Logger::CommentRuntime("success | save scene");
		}

		ImGui::EndMenu();
	}
}

void HierarchyEditor::ShowHierarchyWindow() {
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Hierarchy ## Hierarchy Editor", nullptr, BaseEditor::GetWindowFlag());

	// hierarchyの表示
	ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0);

	for (auto& behaviour : sMonoBehaviourContainer->GetContainer()) {
		HierarchySelectable(behaviour);
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
		ImGui::InvisibleButton("## DropTarget UContentModel", contentSize);

		sUContentStorage->DragAndDropTargetContentFunc<UContentModel>([this](const std::shared_ptr<UContentModel>& content) {
			std::unique_ptr<SceneObject> object = std::make_unique<SceneObject>();
			object->SetName(content->GetFilepath().stem().generic_string());

			ComponentHelper::CreateStaticMeshBehaviour(object.get(), content);

			sSceneObjects->AddObject(std::move(object));
		});

		ImGui::SetCursorScreenPos(contentPos);
		ImGui::InvisibleButton("## DropTarget Scene", contentSize);

		sUContentStorage->DragAndDropTargetContentFunc<UContentScene>([this](const std::shared_ptr<UContentScene>& content) {
			content->WaitComplete(); // contentの読み込みを待つ
			sSceneObjects->InputJson(content->GetData());
			sceneFileName_ = content->GetFilepath().filename();
			sceneNameBuf_  = sceneFileName_.generic_string();
		});
	}

	ImGui::End();
}

void HierarchyEditor::LateUpdate() {
	sSceneObjects->Update();
}

void HierarchyEditor::ForEachBehaviourHierarchy(const MonoBehaviour::Hierarchy& hierarchy, const std::function<void(MonoBehaviour*)>& function) {
	// child持ちのbehaviour
	for (auto& child : hierarchy) {
		auto ptr = std::visit(MonoBehaviour::GetPtrVisitor{}, child);
		if (ptr->HasChild()) {
			function(ptr);
		}
	}

	// rootのbehaviour
	for (auto& child : hierarchy) {
		auto ptr = std::visit(MonoBehaviour::GetPtrVisitor{}, child);
		if (!ptr->HasChild()) {
			function(ptr);
		}
	}
}

void HierarchyEditor::HierarchySelectable(MonoBehaviour* behaviour) {

	bool isSelect     = CheckSelected(behaviour);
	std::string label = std::format("{} # {:p}", behaviour->GetName(), static_cast<const void*>(behaviour));

	bool hasChild = !behaviour->GetChildren().empty();

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

	if (ImGui::IsItemClicked()) {
		SetSelected(behaviour);
		isSelect = true;
	}

	if (isSelect && SxImGui::IsDoubleClickItem()) {
		SetSelectedView(behaviour);
	}

	if (isOpen) {
		ForEachBehaviourHierarchy(behaviour->GetChildren(), [this](MonoBehaviour* child) {
			HierarchySelectable(child);
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
