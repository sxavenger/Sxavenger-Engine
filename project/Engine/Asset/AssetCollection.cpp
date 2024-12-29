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

	root_.first = "root";
}

void AssetCollection::ShowAssetTree() {

	std::function<void(FolderPair*)> folderFunction = [&](FolderPair* node) {
		//!< folder全探索
		for (auto& folder : node->second.folder.GetMap()) {
			if (ImGui::TreeNodeEx(folder.first.string().c_str(), ImGuiTreeNodeFlags_OpenOnArrow)) {
				folderFunction(&folder.second);
				ImGui::TreePop();
			}
		}

		//!< fileの表示
		for (auto& file : node->second.files.GetMap()) {
			ImGui::Selectable(file.first.string().c_str());
		}
	};

	folderFunction(&root_);

}

AssetCollection::FolderPair* AssetCollection::GetFolder(const std::filesystem::path& folderpath) {

	FolderPair* current = &root_;

	for (auto itr = folderpath.begin(); itr != folderpath.end(); ++itr) {
		if (itr == folderpath.begin() && *itr == root_.first) {
			continue;
		}

		Assert(current->second.folder.Contains(*itr), "folder is not found.");
		current = &current->second.folder[*itr];
	};

	return current;
}

std::filesystem::path AssetCollection::ToLower(const std::filesystem::path& path) {

	std::filesystem::path lowerpath;

	for (const auto& part : path) {
		std::string str = part.string();

		std::transform(
			str.begin(), str.end(),
			str.begin(),
			[](unsigned char c) {
				return static_cast<char>(std::tolower(c));
			}
		);

		lowerpath /= str;
	}

	return lowerpath;
}

AssetCollection::FileType AssetCollection::GetFileType(const std::filesystem::path& filepath) {

	const auto extension = filepath.extension();

	if (extension == ".png" || extension == ".jpg" || extension == ".dds") {
		return FileType::Texture;

	} else if (extension == ".obj" || extension == ".gltf" || extension == ".fbx") {
		return FileType::ModelAnimator;

	} else if (extension == ".hlsl" || extension == ".hlsli") {
		return FileType::Shader;
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

		case FileType::Shader:
			file.second = std::make_shared<AssetBlob>();
			break;

		case FileType::ModelAnimator:
			file.second = std::make_shared<ModelAnimator>();
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
	FolderPair* current = &root_;

	std::filesystem::path folderpath = root_.first;

	//!< filenameが含まれているフォルダまで移動または作成
	for (const auto& path : filepath) {
		folderpath /= ToLower(path);

		if (path == filename) {
			break;
		}

		current->second.folder[path];
		current->second.folder[path].first = folderpath;

		current = &current->second.folder[path];
	}

	File file = CreateFileData(filepath);
	file.second->SetCollection(this);

	current->second.files[filename] = file;
	return current->second.files.At(filename);
}

const AssetCollection::File& AssetCollection::TryImportFile(const std::filesystem::path& filepath) {

	const auto filename = filepath.filename();
	FolderPair* current = &root_;

	std::filesystem::path folderpath = root_.first;

	//!< filenameが含まれているフォルダまで移動または作成
	for (const auto& path : filepath) {
		folderpath /= ToLower(path);

		if (path == filename) {
			break;
		}

		current->second.folder[path];
		current->second.folder[path].first = folderpath;

		current = &current->second.folder[path];
	}

	//!< importされていない場合, 作成
	if (!current->second.files.Contains(filename)) {

		File file = CreateFileData(filepath);
		file.second->SetCollection(this);

		current->second.files.Emplace(filename, file);
	}

	return current->second.files.At(filename);
}

const AssetCollection::File& AssetCollection::GetFile(const std::filesystem::path& filepath) const {

	const auto filename       = filepath.filename();
	const FolderPair* current = &root_;

	//!< filenameが含まれているフォルダまで移動または作成
	for (const auto& path : filepath) {

		if (path == filename) {
			break;
		}

		current = &current->second.folder.At(path);
	}

	return current->second.files.At(filename);
}
