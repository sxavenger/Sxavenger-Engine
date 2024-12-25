#include "AssetCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetCollection::Init() {
	AsyncAssetThreadCollection::Init(2);
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
