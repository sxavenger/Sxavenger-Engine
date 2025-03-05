#include "EditorEngine.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//=========================================================================================
// static variables
//=========================================================================================

const std::string EditorEngine::kEditorName_ = "Sxavenger Engine Editor";

////////////////////////////////////////////////////////////////////////////////////////////
// EditorEngine class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EditorEngine::Init() {
	windowFlag_ = 0;
	windowFlag_ |= ImGuiWindowFlags_NoCollapse;
	windowFlag_ |= ImGuiWindowFlags_NoMove;
	windowFlag_ |= ImGuiWindowFlags_NoResize;
}

void EditorEngine::Term() {
	editors_.clear();
}

void EditorEngine::UpdateEditor() {
#ifdef _DEVELOPMENT
	ShowMainMenu();
	ShowWindow();
#endif
}

void EditorEngine::SetNextWindowDocking() const {
	ImGui::SetNextWindowDockID(dockingId_, ImGuiCond_FirstUseEver);
}

EditorEngine* EditorEngine::GetInstance() {
	static EditorEngine instance;
	return &instance;
}

void EditorEngine::ShowMainMenu() {
	ImGui::BeginMainMenuBar();

	ShowEditorMenu();

	for (const auto& [typeindex, editor] : editors_) {
		editor->ShowMainMenu();
	}

	ImGui::EndMainMenuBar();
}

void EditorEngine::ShowWindow() {
	if (!isEditorDisplay_) {
		return;
	}

	dockingId_ = ImGui::GetID(kEditorName_.c_str());

	for (const auto& [typeindex, editor] : editors_) {
		if (editor->IsDisplay()) {
			editor->ShowWindow();
		}
	}
}

void EditorEngine::MenuDummy() {
	ImGui::Dummy({ 240.0f, 0 });
}

void EditorEngine::ShowEditorMenu() {
	if (ImGui::BeginMenu("editor")) {
		MenuDummy();

		if (ImGui::BeginMenu("config")) {
			MenuDummy();

			ImGui::SeparatorText("config");
			ImGui::Checkbox("display", &isEditorDisplay_);

			ImGui::SeparatorText("window");

			ImGuiWindowFlags lock = 0;
			lock |= ImGuiWindowFlags_NoMove;
			lock |= ImGuiWindowFlags_NoResize;
			ImGui::CheckboxFlags("lock", &windowFlag_, lock);

			if (ImGui::Button("output")) {
				SxavengerSystem::GetImGuiController()->OutputLayout();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("layout")) {
			MenuDummy();
			ImGui::ShowStyleEditor();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("other")) {
			MenuDummy();
			ImGui::SeparatorText("other");

			if (ImGui::BeginTable("## other editor", 3, ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
				ImGui::TableSetupColumn("typeindex");
				ImGui::TableSetupColumn("ptr");
				ImGui::TableSetupColumn("display");
				ImGui::TableHeadersRow();

				for (const auto& [typeindex, editor] : editors_) {
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					ImGui::Text(typeindex.name());
					ImGui::SameLine();
					ImGui::Dummy({ 16.0f, 0.0f });

					std::string ptr = std::format("{:p}", reinterpret_cast<const void*>(editor.get()));

					ImGui::TableNextColumn();
					ImGui::Text(ptr.c_str());
					ImGui::SameLine();
					ImGui::Dummy({ 16.0f, 0.0f });

					ImGui::TableNextColumn();
					ImGui::Checkbox(std::format("## {}", ptr).c_str(), &editor->GetIsDisplay());
					ImGui::SameLine();
					ImGui::Dummy({ 16.0f, 0.0f });
				}

				ImGui::EndTable();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}



