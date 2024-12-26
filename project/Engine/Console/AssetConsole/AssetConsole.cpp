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

void AssetConsole::InitTexture() {
	textures_[TextureType::File]       = SxavengerAsset::TryImportPtr<Texture>("Packages/textures/file.png").lock();
	textures_[TextureType::FilePng]    = SxavengerAsset::TryImportPtr<Texture>("Packages/textures/file_png.png").lock();
	textures_[TextureType::Folder]     = SxavengerAsset::TryImportPtr<Texture>("Packages/textures/folder.png").lock();
	textures_[TextureType::FolderOpen] = SxavengerAsset::TryImportPtr<Texture>("Packages/textures/folder_open.png").lock();

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

		std::function<void(AssetCollection::Folder*)> folderFunction = [&](AssetCollection::Folder* node) {

			//!< folder全探索
			for (auto& folder : node->folder.GetMap()) {

				//!< selected
				bool isSelected = folder_.has_value() && folder_.value() == &folder.second;

				//!< node flag
				ImGuiTreeNodeFlags nodeFlag
					= ImGuiTreeNodeFlags_OpenOnDoubleClick
					| ImGuiTreeNodeFlags_OpenOnArrow;

				if (isSelected) {
					nodeFlag |= ImGuiTreeNodeFlags_Selected;
				}

				//!< image
				float spacing = 16;
				ImGui::Image(textures_[TextureType::Folder]->GetGPUHandleSRV().ptr, { spacing, spacing });
				ImGui::SameLine();

				bool isOpenTreeNode = ImGui::TreeNodeEx(folder.first.string().c_str(), nodeFlag);

				if (ImGui::IsItemClicked()) {
					folder_ = &folder.second;
				}

				if (isOpenTreeNode) {
					folderFunction(&folder.second);
					ImGui::TreePop();
				}
			}

			//!< fileの表示
			for (auto& file : node->files.GetMap()) {

				float spacing = 16;
				ImGui::Image(textures_[TextureType::File]->GetGPUHandleSRV().ptr, { spacing, spacing });
				ImGui::SameLine();

				if (ImGui::Selectable(file.first.string().c_str())) {
					// todo: file select
				}
			}
		};

		folderFunction(&SxavengerAsset::GetCollection()->GetRoot());

		ImGui::EndChild();
	}

	{ //!< Folder Project

		if (folder_.has_value()) {
			for (auto& folder : folder_.value()->folder.GetMap()) {
				ImGui::SameLine();

				if (ImGui::ImageButton(folder.first.generic_string().c_str(), textures_[TextureType::Folder]->GetGPUHandleSRV().ptr, {24, 24})) {
					folder_ = &folder.second;
				}
			}

			for (auto& file : folder_.value()->files.GetMap()) {
				ImGui::SameLine();

				if (ImGui::ImageButton(file.first.generic_string().c_str(), textures_[TextureType::File]->GetGPUHandleSRV().ptr, { 24, 24 })) {
				}
			}
		}

	}

	ImGui::End();
}

void AssetConsole::MenuDummy() {
	static const ImVec2 size = { 240.0f, 0.0f };
	ImGui::Dummy(size);
}
