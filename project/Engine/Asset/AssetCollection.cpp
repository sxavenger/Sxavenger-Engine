#include "AssetCollection.h"

std::weak_ptr<BaseAsset> AssetCollection::Import(const std::filesystem::path& filepath) {

	const auto filename = filepath.filename();

	Folder* current = &root_;

	//!< filenameが含まれているフォルダまで移動または作成
	for (const auto& path : filepath) {
		if (path == filename) {
			break;
		}

		current = &current->folder[path];
	}

	current->files.TryEmplace(filename, CreateFileData(filepath));
	return current->files.At(filename).second;
}

std::weak_ptr<Texture> AssetCollection::ImportTexture(const std::filesystem::path& filepath) {
	const auto asset = Import(filepath);
	return ConvertAsset<Texture>(asset.lock());
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

	return file;
}
