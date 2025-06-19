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

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 4, 4 });

	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Asset ## Engine Asset Editor", nullptr, BaseEditor::GetWindowFlag());

	//* Asset Directory *//

	ImVec2 context = ImGui::GetContentRegionAvail();

	ImGui::BeginChild("## asset directory", { 160, context.y }, ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders);
	ShowAssetDirectoryTable(kAssetsDirectory);
	ShowAssetDirectoryTable(kPackagesDirectory);
	ImGui::EndChild();

	ImGui::SameLine();

	//* Asset Layout *//

	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
	ImGui::BeginChild("## asset layout", { context.x - ImGui::GetCursorPosX(), context.y });

	ShowAssetDirectoryNode();
	ImGui::Separator();
	ShowAssetLayout();

	ImGui::EndChild();
	ImGui::PopStyleColor();

	
	//!< 廃止予定
	for (const auto& [type, map] : sAssetStorage->GetStorage()) {
		if (ImGui::TreeNode(type->name())) {

			for (const auto& [filepath, asset] : map) {
				if (ImGui::Selectable(filepath.generic_string().c_str(), CheckSelectedInspector(asset.get()))) {
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

	ImGui::End();

	ImGui::PopStyleVar();

}

void AssetEditor::SetSelected(BaseAsset* asset) {
	if (auto editor = BaseEditor::GetEditorEngine()->GetEditor<InspectorEditor>()) {
		editor->SetInspector(asset);
	}
}

bool AssetEditor::CheckSelectedInspector(BaseAsset* asset) {
	if (auto editor = BaseEditor::GetEditorEngine()->GetEditor<InspectorEditor>()) {
		return editor->CheckInspector(asset);
	}

	return false;
}


void AssetEditor::ShowAssetDirectoryTable(const std::filesystem::path& path) {

	bool isDirectory   = std::filesystem::is_directory(path);
	std::u8string name = path.filename().generic_u8string();
	bool isSelected    = selectedDirectory_.has_value() && selectedDirectory_.value() == path;

	ImGuiTreeNodeFlags flags
		= ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_FramePadding
		| ImGuiTreeNodeFlags_SpanAllColumns
		| ImGuiTreeNodeFlags_DrawLinesToNodes;

	if (isSelected) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	if (!isDirectory) {
		flags |= ImGuiTreeNodeFlags_Leaf;
		ImGui::Unindent();
	}

	bool isOpen = ImGui::TreeNodeEx(reinterpret_cast<const char*>(name.c_str()), flags);

	if (!isDirectory) {
		ImGui::Indent();
	}

	if (ImGui::IsItemClicked() && isDirectory) {
		selectedDirectory_ = path;
	}

	if (isOpen) {
		if (isDirectory) {
			for (const auto& entry : std::filesystem::directory_iterator(path)) {
				ShowAssetDirectoryTable(entry.path());
			}
		}

		ImGui::TreePop();
	}
}

void AssetEditor::ShowAssetDirectoryNode() {
	if (!selectedDirectory_.has_value()) {
		return;
	}

	const auto& path = selectedDirectory_.value();

	//* node directory *//

	if (std::filesystem::exists(path)) {

		ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 0);
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });

		std::filesystem::path current;
		std::optional<std::filesystem::path> select;

		for (const auto& part : path) {
			current /= part;

			if (ImGui::Button(current.filename().generic_string().c_str())) {
				select = current;
			}

			if (current != path) {
				ImGui::SameLine();
				ImGui::Text("/");
				ImGui::SameLine();
			}
		}

		if (select.has_value()) {
			selectedDirectory_ = select.value();
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

	} else {
		ImGui::Text("??? [selected directory path is not exists.]");
	}
}

void AssetEditor::ShowAssetLayout() {
	if (!selectedDirectory_.has_value()) {
		return;
	}

	const auto& path = selectedDirectory_.value();


}
