#include "AssetCollection.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetCollection::Init() {
	AsyncAssetThreadCollection::Init(2);
}

void AssetCollection::ShowAssetTree() {

	std::function<void(Folder*)> folderFunction = [&](Folder* node) {
		//!< folder全探索
		for (auto& folder : node->folder.GetMap()) {
			if (ImGui::TreeNodeEx(folder.first.string().c_str(), ImGuiTreeNodeFlags_OpenOnArrow)) {
				folderFunction(&folder.second);
				ImGui::TreePop();
			}
		}

		//!< fileの表示
		for (auto& file : node->files.GetMap()) {
			ImGui::Selectable(file.first.string().c_str());
		}
	};

	folderFunction(&root_);

}

AssetCollection::FileType AssetCollection::GetFileType(const std::filesystem::path& filepath) {

	const auto extension = filepath.extension();

	if (extension == ".png" || extension == ".jpg" || extension == ".dds") {
		return FileType::Texture;

	} else if (extension == ".obj" || extension == ".gltf" || extension == ".fbx") {
		return FileType::Model;
	}

	return FileType::Unknown;
}

AssetCollection::File AssetCollection::CreateFileData(const std::filesystem::path& filepath) {

	File file = {};

	file.first = GetFileType(filepath);

	switch (file.first) {
		case FileType::Texture:
			file.second = std::make_shared<Texture>();
			break;

		default:
			file.second = std::make_shared<AssetUnknown>();
			break;
	}

	file.second->SetFilepath(filepath);
	return file;
}

const AssetCollection::File& AssetCollection::ImportFile(const std::filesystem::path& filepath) {

	const auto filename = filepath.filename();
	Folder* current = &root_;

	//!< filenameが含まれているフォルダまで移動または作成
	for (const auto& path : filepath) {
		if (path == filename) {
			break;
		}

		current = &current->folder[path];
	}

	File file = CreateFileData(filepath);
	file.second->SetCollection(this);

	current->files[filename] = file;

	return current->files.At(filename);
}

const AssetCollection::File& AssetCollection::TryImportFile(const std::filesystem::path& filepath) {

	const auto filename = filepath.filename();
	Folder* current = &root_;

	//!< filenameが含まれているフォルダまで移動または作成
	for (const auto& path : filepath) {
		if (path == filename) {
			break;
		}

		current = &current->folder[path];
	}

	//!< importされていない場合, 作成
	if (!current->files.Contains(filename)) {

		File file = CreateFileData(filepath);
		file.second->SetCollection(this);

		current->files.Emplace(filename, file);
	}

	return current->files.At(filename);
}

const AssetCollection::File& AssetCollection::GetFile(const std::filesystem::path& filepath) const {

	const auto filename   = filepath.filename();
	const Folder* current = &root_;

	//!< filenameが含まれているフォルダまで移動または作成
	for (const auto& path : filepath) {
		if (path == filename) {
			break;
		}

		current = &current->folder.At(path);
	}

	return current->files.At(filename);
}
