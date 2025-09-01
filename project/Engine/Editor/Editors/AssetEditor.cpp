#include "AssetEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "../EditorEngine.h"
#include "InspectorEditor.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/Preview/Content/UContentStorage.h>

//////////////////////////////////////////////////////////////////////////////////////////
// AssetEditor class methods
//////////////////////////////////////////////////////////////////////////////////////////

void AssetEditor::Init() {
	folderTextures_ = sUContentStorage->Import<UContentTexture>("Packages/textures/icon/folder.png")->GetId();
	fileTexture_    = sUContentStorage->Import<UContentTexture>("Packages/textures/icon/file.png")->GetId();

	assetTextures_[&typeid(UContentTexture)] = sUContentStorage->Import<UContentTexture>("Packages/textures/icon/texture.png")->GetId();
	assetTextures_[&typeid(UContentModel)]   = sUContentStorage->Import<UContentTexture>("Packages/textures/icon/model.png")->GetId();
	assetTextures_[&typeid(UContentBlob)]    = sUContentStorage->Import<UContentTexture>("Packages/textures/icon/shader.png")->GetId();
	assetTextures_[&typeid(UContentScene)]   = sUContentStorage->Import<UContentTexture>("Packages/textures/icon/scene.png")->GetId();

	// extensionの登録
	// todo: extensionからiconを変更
	RegisterExtension<UContentTexture>(".png");
	RegisterExtension<UContentTexture>(".jpg");
	RegisterExtension<UContentTexture>(".jpeg");
	RegisterExtension<UContentTexture>(".tga");
	RegisterExtension<UContentModel>(".gltf");
	RegisterExtension<UContentModel>(".obj");
	RegisterExtension<UContentModel>(".fbx");
	RegisterExtension<UContentScene>(".scene");
	RegisterExtension<UContentFont>(".ttf");

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

bool AssetEditor::ImageButton(const std::filesystem::path& path, const UAssetParameter<UAssetTexture>& texture) {
	return ImGui::ImageButton(ConvertStr(path).c_str(), texture.WaitRequire()->GetGPUHandleSRV().ptr, { iconSize_, iconSize_ });
}

const std::string AssetEditor::ConvertStr(const std::filesystem::path& path) {
	std::u8string name = path.filename().generic_u8string();
	return std::string(name.begin(), name.end());
}

void AssetEditor::SetSelected(UBaseContent* content) {
	if (auto editor = BaseEditor::GetEditorEngine()->GetEditor<InspectorEditor>()) {
		editor->SetInspector(content);
	}
}

bool AssetEditor::CheckSelectedInspector(UBaseContent* content) {
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
		if (entry.path().extension() == UBaseContent::GetContentExtension()) {
			continue; // contentファイルは除外
		}

		func(entry);
	}
}

bool AssetEditor::OpenShellExecuteApp(const std::filesystem::path& filepath) {
	HINSTANCE result = ShellExecute(nullptr, L"open", filepath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

	bool isSuccess = (reinterpret_cast<intptr_t>(result) > 32);

	if (isSuccess) {
		Logger::CommentRuntime("shell execute application.", filepath.generic_string());

	} else {
		Logger::WarningRuntime("warning | shell execute application failed.", filepath.generic_string());
	}

	return isSuccess;
}

bool AssetEditor::OpenShellExecuteExplorer(const std::filesystem::path& filepath) {
	// FIXME: explorerで開けない
	HINSTANCE result = ShellExecute(nullptr, L"open", L"explorer.exe", filepath.wstring().c_str(), nullptr, SW_SHOWNORMAL);

	bool isSuccess = (reinterpret_cast<intptr_t>(result) > 32);

	if (isSuccess) {
		Logger::CommentRuntime("shell execute explorer.", filepath.generic_string());

	} else {
		Logger::WarningRuntime("warning | shell execute explorer failed.", filepath.generic_string());
	}

	return isSuccess;
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
					OpenShellExecuteExplorer(part);
				}

				ImGui::EndPopup();
			}


		} else {
			if (auto type = sUContentStorage->GetType(part)) { //!< Assetとして読み込まれている場合

				auto& texture = assetTextures_.contains(type)
					? assetTextures_[type] //!< asset texture
					: fileTexture_;        //!< unknown asset type

				if (ImageButton(part, texture)) {
					SetSelected(sUContentStorage->GetContent(type, part).get());
				}

				// sourceの設定
				sUContentStorage->DragAndDropSource(type, part);

			} else {
				if (ImageButton(part, fileTexture_)) {
					//
				}
			}

			if (SxImGui::IsDoubleClickItem()) {
				OpenShellExecuteApp(part);
			}

			std::string label = "file context menu ##" + part.filename().generic_string();
			if (ImGui::BeginPopupContextItem(label.c_str(), ImGuiPopupFlags_MouseButtonRight)) {
				ImGui::SeparatorText("file context menu");

				if (ImGui::Selectable("Open in Explorer")) {
					OpenShellExecuteExplorer(part);
				}

				if (ImGui::Selectable("Import")) {
					if (extensions_.contains(part.extension())) {
						const auto& [type, function] = extensions_.at(part.extension());

						std::shared_ptr<UBaseContent> content = function(part);
						sUContentStorage->TryEmplace(type, content);
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
