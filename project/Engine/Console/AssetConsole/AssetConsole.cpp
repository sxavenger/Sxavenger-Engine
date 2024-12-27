#include "AssetConsole.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* console
#include "../Console.h"

//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SystemConsole class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetConsole::Init(Console* console) {
	console_ = console;

	InitTexture();

	folder_ = SxavengerAsset::GetCollection()->GetRoot();

	std::filesystem::path path = "packages/tmp";

	AssetCollection::FolderPair* folder = SxavengerAsset::GetCollection()->GetRoot();

	for (const auto& part : path) {
		folder = &folder->second.folder[part];
	}
}

void AssetConsole::Term() {
	std::for_each(textures_.begin(), textures_.end(), [](std::shared_ptr<Texture>& texture) { texture.reset(); });
}

void AssetConsole::UpdateConsole() {
	if (isDisplayAssetConsole_) {
		//DisplayAsset();
		DisplayProject();
	}
}

void AssetConsole::ShowAssetMenu() {
	if (ImGui::BeginMenu("Thread")) {
		MenuDummy();
		SxavengerAsset::GetCollection()->SystemDebugGui();
		ImGui::EndMenu();
	}

}

AssetConsole::TextureType AssetConsole::GetTextureType(const AssetCollection::File& file) {
	switch (file.first) {
		case AssetCollection::FileType::Texture:
			return TextureType::File_Texture;
			break;

		case AssetCollection::FileType::ModelAnimator:
			return TextureType::File_ModelAnimator;
			break;

		case AssetCollection::FileType::Shader:
			return TextureType::File_Shader;
			break;
	}

	return TextureType::File;
}

void AssetConsole::InitTexture() {
	//!< file
	textures_[TextureType::File]               = SxavengerAsset::TryImportPtr<Texture>("Packages/textures/tree/file.png").lock();
	textures_[TextureType::File_Texture]       = SxavengerAsset::TryImportPtr<Texture>("Packages/textures/tree/file_texture.png").lock();
	textures_[TextureType::File_ModelAnimator] = SxavengerAsset::TryImportPtr<Texture>("Packages/textures/tree/file_model.png").lock();
	textures_[TextureType::File_Shader]        = SxavengerAsset::TryImportPtr<Texture>("Packages/textures/tree/file_shader.png").lock();

	//!< folder
	textures_[TextureType::Folder]            = SxavengerAsset::TryImportPtr<Texture>("Packages/textures/tree/folder.png").lock();
	textures_[TextureType::Folder_Open]       = SxavengerAsset::TryImportPtr<Texture>("Packages/textures/tree/folder_open.png").lock();
	textures_[TextureType::Folder_Empty]      = SxavengerAsset::TryImportPtr<Texture>("Packages/textures/tree/folder_empty.png").lock();
	textures_[TextureType::Folder_Empty_Open] = SxavengerAsset::TryImportPtr<Texture>("Packages/textures/tree/folder_empty_open.png").lock();

	std::for_each(textures_.begin(), textures_.end(), [](std::shared_ptr<Texture>& texture) { texture->Load(SxavengerSystem::GetMainThreadContext()); });
}

void AssetConsole::DisplayProject() {
	console_->DockingConsole();
	ImGui::Begin("Project ## Asset Console", nullptr, console_->GetWindowFlag());

	{ //!< Folder Tree
		ImGuiChildFlags flag = 0;
		flag |= ImGuiChildFlags_ResizeX;
		flag |= ImGuiChildFlags_FrameStyle;

		ImVec2 region = ImGui::GetContentRegionAvail();

		ImGui::BeginChild("asset tree", { region.x * 0.2f, region.y }, flag, ImGuiWindowFlags_AlwaysVerticalScrollbar);

		std::function<void(AssetCollection::FolderPair*)> folderFunction = [&](AssetCollection::FolderPair* node) {

			//!< folder全探索
			for (auto& [part, folder] : node->second.folder.GetMap()) {

				//!< selected
				bool isSelected = folder_.has_value() && folder_.value() == &folder;

				//!< node flag
				ImGuiTreeNodeFlags nodeFlag
					= ImGuiTreeNodeFlags_OpenOnDoubleClick
					| ImGuiTreeNodeFlags_OpenOnArrow;

				if (isSelected) {
					nodeFlag |= ImGuiTreeNodeFlags_Selected;
				}

				TextureType type = TextureType::Folder;

				if (folder.second.Empty()) {
					if (isSelected) {
						type = TextureType::Folder_Empty_Open;

					} else {
						type = TextureType::Folder_Empty;
					}

				} else {
					if (isSelected) {
						type = TextureType::Folder_Open;

					} else {
						type = TextureType::Folder;
					}
				}

				//!< image
				float spacing = 16;
				ImGui::Image(textures_[type]->GetGPUHandleSRV().ptr, { spacing, spacing });
				ImGui::SameLine();

				ImVec2 cursol = ImGui::GetCursorPos();
				ImGui::SetCursorPos({ cursol.x - 4, cursol.y });
				// todo: icon描画関数を用意
				// todo: colorを入れる

				bool isOpenTreeNode = ImGui::TreeNodeEx(part.string().c_str(), nodeFlag);

				if (ImGui::IsItemClicked()) {
					folder_ = &folder;
				}

				if (isOpenTreeNode) {
					folderFunction(&folder);
					ImGui::TreePop();
				}
			}

			//!< fileの表示
			for (auto& [part, file] : node->second.files.GetMap()) {

				TextureType type = GetTextureType(file);

				float spacing = 16;
				ImGui::Image(textures_[type]->GetGPUHandleSRV().ptr, { spacing, spacing });
				ImGui::SameLine();

				ImVec2 cursol = ImGui::GetCursorPos();
				ImGui::SetCursorPos({ cursol.x - 4, cursol.y });

				if (ImGui::Selectable(part.string().c_str())) {
					// todo: file select
				}
			}
		};

		folderFunction(SxavengerAsset::GetCollection()->GetRoot());

		ImGui::EndChild();
	}

	{ //!< Folder Project

		if (folder_.has_value()) {

			ImGui::SameLine();
			ImVec2 beginCursol = ImGui::GetCursorPos();

			//!< folder node button
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			std::filesystem::path folderpath = "";
			for (const auto& part : folder_.value()->first) {
				folderpath /= part;

				ImGui::SameLine();

				if (ImGui::Button(part.generic_string().c_str())) {
					folder_ = SxavengerAsset::GetCollection()->GetFolder(folderpath);
				}

				ImGui::SameLine();

				ImGui::Text("/");
			}

			ImGui::PopStyleColor();

			ImVec2 rect    = ImGui::GetItemRectSize();
			ImVec2 sapcing = ImGui::GetStyle().ItemSpacing;

			ImGui::SetCursorPos({ beginCursol.x, beginCursol.y + rect.y + sapcing.y * 2.0f });

			//!< folder and file button
			for (auto& folder : folder_.value()->second.folder.GetMap()) {

				if (ImGui::ImageButton(folder.first.generic_string().c_str(), textures_[TextureType::Folder]->GetGPUHandleSRV().ptr, {24, 24})) {
					folder_ = &folder.second;
				}

				ImGui::SameLine();
			}

			for (auto& file : folder_.value()->second.files.GetMap()) {

				TextureType type = GetTextureType(file.second);

				if (ImGui::ImageButton(file.first.generic_string().c_str(), textures_[type]->GetGPUHandleSRV().ptr, { 24, 24 })) {
				}

				ImGui::SameLine();
			}
		}

	}

	ImGui::End();
}

void AssetConsole::MenuDummy() {
	static const ImVec2 size = { 240.0f, 0.0f };
	ImGui::Dummy(size);
}
