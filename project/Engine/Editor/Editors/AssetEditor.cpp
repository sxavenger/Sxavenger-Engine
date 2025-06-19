#include "AssetEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "../EditorEngine.h"
#include "InspectorEditor.h"

//////////////////////////////////////////////////////////////////////////////////////////
// AssetEditor class methods
//////////////////////////////////////////////////////////////////////////////////////////

void AssetEditor::Init() {

}

void AssetEditor::ShowMainMenu() {
}

void AssetEditor::ShowWindow() {
	ShowAssetWindow();
}

void AssetEditor::ShowAssetWindow() {

	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Asset ## Engine Asset Editor", nullptr, BaseEditor::GetWindowFlag());

	//* Asset Directory *//

	ImVec2 context = ImGui::GetContentRegionAvail();

	ImGui::BeginChild("## asset directory", { 160, context.y }, ImGuiChildFlags_ResizeX);
	ShowAssetDirectory(kAssetsDirectory);
	ShowAssetDirectory(kPackagesDirectory);
	ImGui::EndChild();

	ImGui::SameLine();

	//* Asset Layout *//

	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
	ImGui::BeginChild("## asset layout", { context.x - ImGui::GetCursorPosX(), context.y });

	// todo: layoutの変更
	for (const auto& [type, map] : sAssetStorage->GetStorage()) {
		if (ImGui::TreeNode(type->name())) {

			for (const auto& [filepath, asset] : map) {
				if (ImGui::Selectable(filepath.generic_string().c_str(), CheckSelected(asset.get()))) {
					SetSelected(asset.get());
				}

				// drag and dropの定義
				if (ImGui::BeginDragDropSource()) {
					// payloadを設定
					ImGui::SetDragDropPayload(type->name(), filepath.generic_string().c_str(), filepath.generic_string().size() + 1);

					// ドラッグ中の表示
					ImGui::Text(type->name());
					ImGui::Text(filepath.generic_string().c_str());

					ImGui::EndDragDropSource();
				}

			}

			ImGui::TreePop();
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleColor();

	ImGui::End();

}

bool AssetEditor::CheckSelected(BaseAsset* asset) {
	if (auto editor = BaseEditor::GetEditorEngine()->GetEditor<InspectorEditor>()) {
		return editor->CheckInspector(asset);
	}

	return false;
}

void AssetEditor::ShowAssetDirectory(const std::filesystem::path& path) {

	bool isDirectory   = std::filesystem::is_directory(path);
	std::u8string name = path.filename().generic_u8string();

	ImGuiTreeNodeFlags flags
		= ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_FramePadding
		| ImGuiTreeNodeFlags_SpanAllColumns
		| ImGuiTreeNodeFlags_DrawLinesToNodes;

	if (!isDirectory) {
		flags |= ImGuiTreeNodeFlags_Leaf;
		ImGui::Unindent();
	}

	bool isOpen = ImGui::TreeNodeEx(reinterpret_cast<const char*>(name.c_str()), flags);

	if (!isDirectory) {
		ImGui::Indent();
	}

	if (isOpen) {
		if (isDirectory) {
			for (const auto& entry : std::filesystem::directory_iterator(path)) {
				ShowAssetDirectory(entry.path());
			}
		}

		ImGui::TreePop();
	}
}

void AssetEditor::SetSelected(BaseAsset* asset) {
	if (auto editor = BaseEditor::GetEditorEngine()->GetEditor<InspectorEditor>()) {
		editor->SetInspector(asset);
	}
}
