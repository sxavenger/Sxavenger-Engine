#include "AssetEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "../EditorEngine.h"
#include "InspectorEditor.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/Preview/Content/ContentStorage.h>

//////////////////////////////////////////////////////////////////////////////////////////
// AssetEditor class methods
//////////////////////////////////////////////////////////////////////////////////////////

void AssetEditor::Init() {
	folderTextures_ = sContentStorage->Import<ContentTexture>("Packages/textures/icon/folder.png")->GetId();
	fileTexture_    = sContentStorage->Import<ContentTexture>("Packages/textures/icon/file.png")->GetId();

	assetTextures_[&typeid(ContentTexture)] = sContentStorage->Import<ContentTexture>("Packages/textures/icon/texture.png")->GetId();
	assetTextures_[&typeid(ContentModel)]   = sContentStorage->Import<ContentTexture>("Packages/textures/icon/model.png")->GetId();
	assetTextures_[&typeid(ContentBlob)]    = sContentStorage->Import<ContentTexture>("Packages/textures/icon/shader.png")->GetId();
	assetTextures_[&typeid(ContentScene)]   = sContentStorage->Import<ContentTexture>("Packages/textures/icon/scene.png")->GetId();

	// extensionの登録
	// todo: extensionからiconを変更
	RegisterExtension<ContentTexture>(".png");
	RegisterExtension<ContentTexture>(".jpg");
	RegisterExtension<ContentTexture>(".jpeg");
	RegisterExtension<ContentTexture>(".tga");
	RegisterExtension<ContentModel>(".gltf");
	RegisterExtension<ContentModel>(".obj");
	RegisterExtension<ContentModel>(".fbx");
	RegisterExtension<ContentScene>(".scene");
	RegisterExtension<ContentFont>(".ttf");
	RegisterExtension<ContentAudio>(".wav");
	RegisterExtension<ContentAudio>(".mp3");

}

void AssetEditor::ShowMainMenu() {
	ShowAssetMenu();
}

void AssetEditor::ShowWindow() {
	ShowAssetWindow();
}

void AssetEditor::ShowAssetMenu() {
	if (ImGui::BeginMenu("asset")) {

		ImGui::SeparatorText("asset storage info");

		ImGui::BeginTable("## asset storage info table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
		ImGui::TableSetupColumn("type");
		ImGui::TableSetupColumn("count");
		ImGui::TableHeadersRow();

		for (const auto& [type, storage] : sAssetStorage->GetStorage()) {
			ImGui::TableNextRow();

			{ //!< type
				ImGui::TableNextColumn();
				ImGui::Text(type->name());
				ImGui::SameLine();
				ImGui::Dummy({ 8.0f, 0.0f });
			}

			{ //!< storage count
				ImGui::TableNextColumn();
				ImGui::Text(std::format("{}", storage.size()).c_str());
				ImGui::SameLine();
				ImGui::Dummy({ 8.0f, 0.0f });
			}
			
		}

		ImGui::EndTable();
		ImGui::EndMenu();
	}
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

	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
		ImGui::BeginChild("## asset layout", { context.x - ImGui::GetCursorPosX(), context.y });

		ShowAssetDirectoryNode();
		ImGui::Separator();
		ShowAssetLayout();

		ImGui::EndChild();
		ImGui::PopStyleColor();
	}

	ImGui::DragFloat("icon size", &iconSize_, 1.0f, 16.0f, 128.0f, "%.0f px", ImGuiSliderFlags_AlwaysClamp);

	ImGui::End();
	ImGui::PopStyleVar();

}

bool AssetEditor::ImageButton(const std::filesystem::path& path, const AssetParameter<AssetTexture>& texture) {
	return ImGui::ImageButton(ConvertStr(path).c_str(), texture.WaitRequire()->GetGPUHandleSRV().ptr, { iconSize_, iconSize_ });
}

const std::string AssetEditor::ConvertStr(const std::filesystem::path& path) {
	std::u8string name = path.filename().generic_u8string();
	return std::string(name.begin(), name.end());
}

void AssetEditor::SetSelected(BaseContent* content) {
	if (auto editor = BaseEditor::GetEditorEngine()->GetEditor<InspectorEditor>()) {
		editor->SetInspector(content);
	}
}

bool AssetEditor::CheckSelectedInspector(BaseContent* content) {
	if (auto editor = BaseEditor::GetEditorEngine()->GetEditor<InspectorEditor>()) {
		return editor->CheckInspector(content);
	}

	return false;
}

void AssetEditor::ForEachDirectory(const std::filesystem::path& path, const std::function<void(const std::filesystem::directory_entry&)>& func) {

	// directory only
	for (const auto& entry : std::filesystem::directory_iterator(path) | std::views::filter([](const std::filesystem::directory_entry& entry) { return entry.is_directory(); })) {
		func(entry);
	}

	// file only
	for (const auto& entry : std::filesystem::directory_iterator(path) | std::views::filter([](const std::filesystem::directory_entry& entry) { return !entry.is_directory(); })) {
		if (entry.path().extension() == BaseContent::GetContentExtension()) {
			continue; // contentファイルは除外
		}

		func(entry);
	}
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

	bool isOpen = ImGui::TreeNodeEx(ConvertStr(path.filename()).c_str(), flags);

	if (!isDirectory) {
		ImGui::Indent();
	}

	if (ImGui::IsItemClicked() && isDirectory) {
		selectedDirectory_ = path;
	}

	if (isOpen) {
		if (isDirectory) {
			ForEachDirectory(
				path,
				[this](const std::filesystem::directory_entry& entry) { ShowAssetDirectoryTable(entry.path()); }
			);
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

			if (ImGui::Button(ConvertStr(current.filename()).c_str())) {
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

	std::optional<std::filesystem::path> select;

	ImVec2 context = ImGui::GetContentRegionAvail();

	size_t space = static_cast<size_t>(context.x) / static_cast<size_t>(iconSize_ + ImGui::GetStyle().ItemSpacing.x);
	size_t count = 0;

	ForEachDirectory(path, [&](const std::filesystem::directory_entry& entry) {
		const auto& part = entry.path();

		ImGui::BeginGroup();

		bool isSameLine = (space != 0) && (++count % space != 0);

		if (entry.is_directory()) {
			if (ImageButton(part, folderTextures_)) {
				select = part;
			}

			std::string label = "folder context menu ##" + part.filename().generic_string();
			if (ImGui::BeginPopupContextItem(label.c_str(), ImGuiPopupFlags_MouseButtonRight)) {
				ImGui::SeparatorText("folder context menu");

				if (ImGui::Selectable("Open in Explorer")) {
					WinApp::OpenExplorer(part);
				}

				ImGui::EndPopup();
			}


		} else {
			if (auto type = sContentStorage->GetType(part)) { //!< Assetとして読み込まれている場合

				auto& texture = assetTextures_.contains(type)
					? assetTextures_[type] //!< asset texture
					: fileTexture_;        //!< unknown asset type

				if (ImageButton(part, texture)) {
					SetSelected(sContentStorage->GetContent(type, part).get());
				}

				// sourceの設定
				sContentStorage->DragAndDropSource(type, part);

			} else {
				if (ImageButton(part, fileTexture_)) {
					//
				}
			}

			if (SxImGui::IsDoubleClickItem()) {
				WinApp::OpenApplication(part);
			}

			std::string label = "file context menu ##" + part.filename().generic_string();
			if (ImGui::BeginPopupContextItem(label.c_str(), ImGuiPopupFlags_MouseButtonRight)) {
				ImGui::SeparatorText("file context menu");

				if (ImGui::Selectable("Open in Explorer")) {
					WinApp::OpenExplorer(part);
				}

				if (ImGui::Selectable("Import")) {
					if (extensions_.contains(part.extension())) {
						const auto& [type, function] = extensions_.at(part.extension());

						std::shared_ptr<BaseContent> content = function(part);
						sContentStorage->TryEmplace(type, content);
					}
				}

				ImGui::EndPopup();
			}
		}

		SxImGui::TextClippedEx(ConvertStr(part.filename()).c_str(), "...", iconSize_);

		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text(ConvertStr(part.filename()).c_str());
			ImGui::EndTooltip();
		}

		ImGui::EndGroup();

		if (isSameLine) {
			ImGui::SameLine();
		}

	});

	if (select.has_value()) {
		selectedDirectory_ = select.value();
	}
}
