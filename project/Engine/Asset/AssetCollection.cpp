#include "AssetCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetCollection::Init() {
	AsyncAssetThreadCollection::Init();
	root_.first = "root";
}

void AssetCollection::Term() {
}

const AssetCollection::FileData& AssetCollection::GetFileData(const std::filesystem::path& filepath) const {

	const Folder* current = &root_.second;
	const auto filename       = filepath.filename();

	for (const auto& part : filepath) {
		if (part == filename) {
			Assert(current->files.Contains(filename), "file is not found.");
			return current->files.At(filename);

		} else {
			Assert(current->folders.Contains(part), "folder is not found.");
			current = &current->folders.At(part).second;
		}
	}

	return {}; //!< error
}
AssetCollection::FileData& AssetCollection::GetFileData(const std::filesystem::path& filepath) {

	Folder* current     = &root_.second;
	const auto filename = filepath.filename();

	for (const auto& part : filepath) {
		if (part == filename) {
			Assert(current->files.Contains(filename), "file is not found.");
			break;

		} else {
			Assert(current->folders.Contains(part), "folder is not found.");
			current = &current->folders[part].second;
		}
	}

	return current->files[filename];
}

AssetCollection::FileType AssetCollection::GetFileType(const std::filesystem::path& filepath) {

	const auto extension = filepath.extension();

	if (extension == ".png" || extension == ".jpg" || extension == ".dds") {
		return FileType::Texture;

	} else if (extension == ".obj" || extension == ".gltf" || extension == ".fbx") {
		return FileType::Model_Animator;

	} else if (extension == ".hlsl" || extension == ".hlsli") {
		return FileType::Shader;
	}

	return FileType::Unknown;
}

AssetCollection::FileData AssetCollection::CreateFileData(const std::filesystem::path& filepath) {
	
}
