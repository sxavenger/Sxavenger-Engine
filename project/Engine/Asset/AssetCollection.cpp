#include "AssetCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetCollection::Init() {
	loader_ = std::make_unique<AsyncThreadLoaderCollection>();
	loader_->Init();
	root_.first = "root";
}

void AssetCollection::Term() {
	loader_.reset();
	root_.second.files.Clear();
	root_.second.folders.Clear();
}

const AssetCollection::FileData& AssetCollection::GetFile(const std::filesystem::path& filepath) const {

	const Folder* current  = &root_.second;
	const FileData* result = nullptr;

	for (const auto& part : filepath) {
		if (part.has_extension()) { //!< fileの場合
			Assert(current->files.Contains(part), "file not found.");
			return current->files.At(part);

		} else {
			Assert(current->folders.Contains(part), "folder not found.");
			current = &current->folders.At(part).second;
		}
	}

	Assert(false, "file not found.");
	return *result; //!< warning対策
}

AssetCollection* AssetCollection::GetInstance() {
	static AssetCollection instance;
	return &instance;
}

AssetCollection::FileData& AssetCollection::RegisterFile(const std::filesystem::path& filepath) {

	Folder* current                = &root_.second;
	std::filesystem::path relative = root_.first;

	FileData* result = nullptr;

	for (const auto& part : filepath) {
		relative /= ToLower(part);

		if (part.has_extension()) { //!< fileの場合
			if (current->files.Contains(part)) {
				result = &current->files.At(part);

			} else {
				// fileの登録
				FileData& file = current->files[part];
				file   = AllocateFileType(relative);

				result = &file;
			}

		} else {
			// folderの登録
			FolderData& folder = current->folders[part];

			// folderの初期化
			if (folder.first.empty()) {
				folder.first = relative;
			}
			
			current = &folder.second;
		}
	}

	Assert(result != nullptr, "file not found.");
	return *result;
	
}

AssetCollection::FileType AssetCollection::GetFileType(const std::filesystem::path& filepath) {

	const auto extension = filepath.extension();

	if (extension == ".png" || extension == ".jpg" || extension == ".dds") {
		return FileType::Texture;

	} else if (extension == ".obj" || extension == ".gltf" || extension == ".fbx") {
		return FileType::Model_Animator;

	} else if (extension == ".hlsl" || extension == ".hlsli") {
		return FileType::Shader;

	} /*else if (extension == ".wav" || extension == ".mp3") {
		return FileType::AudioBuffer;
	}*/

	return FileType::Unknown;
}


AssetCollection::FileData AssetCollection::AllocateFileType(const std::filesystem::path& filepath) {
	FileData file = {};
	file.first = GetFileType(filepath);
	return file;
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

std::shared_ptr<AssetTexture> AssetCollection::LoadTexture(FileData& fileData, const std::filesystem::path& filepath) {
	auto& [type, file] = fileData;
	Assert(type == FileType::Texture, "different type.");

	auto asset = std::make_shared<AssetTexture>();
	file       = asset;

	asset->SetFilepath(filepath);
	loader_->PushTask(asset);
	return asset;
}

std::shared_ptr<AssetModel> AssetCollection::LoadModel(FileData& fileData, const std::filesystem::path& filepath) {
	auto& [type, file] = fileData;
	Assert(type == FileType::Model_Animator, "different type.");

	if (std::holds_alternative<std::pair<std::shared_ptr<AssetModel>, std::shared_ptr<AssetAnimator>>>(file)) { //!< modelまたはanimatorが存在する場合
		auto& [model, animator] = std::get<std::pair<std::shared_ptr<AssetModel>, std::shared_ptr<AssetAnimator>>>(file);
		model                   = std::make_shared<AssetModel>();

		model->SetFilepath(filepath);
		model->SetAssimpOption(assimpOptionModel_);
		loader_->PushTask(model);
		return model;

	} else { //!< modelまたはanimatorが存在しない場合
		auto asset = std::make_shared<AssetModel>();
		file       = std::make_pair(asset, nullptr);

		asset->SetFilepath(filepath);
		asset->SetAssimpOption(assimpOptionModel_);
		loader_->PushTask(asset);
		return asset;
	}
}

std::shared_ptr<AssetAnimator> AssetCollection::LoadAnimator(FileData& fileData, const std::filesystem::path& filepath) {
	auto& [type, file] = fileData;
	Assert(type == FileType::Model_Animator, "different type.");

	if (std::holds_alternative<std::pair<std::shared_ptr<AssetModel>, std::shared_ptr<AssetAnimator>>>(file)) { //!< modelまたはanimatorが存在する場合
		auto& [model, animator] = std::get<std::pair<std::shared_ptr<AssetModel>, std::shared_ptr<AssetAnimator>>>(file);
		animator                = std::make_shared<AssetAnimator>();

		animator->SetFilepath(filepath);
		animator->SetAssimpOption(assimpOptionAnimator_);
		loader_->PushTask(animator);
		return animator;

	} else { //!< modelまたはanimatorが存在しない場合
		auto asset = std::make_shared<AssetAnimator>();
		file       = std::make_pair(nullptr, asset);

		asset->SetFilepath(filepath);
		asset->SetAssimpOption(assimpOptionAnimator_);
		loader_->PushTask(asset);
		return asset;
	}
}

std::shared_ptr<AssetBlob> AssetCollection::LoadBlob(FileData& fileData, const std::filesystem::path& filepath) {
	auto& [type, file] = fileData;
	Assert(type == FileType::Shader, "different type.");

	auto asset = std::make_shared<AssetBlob>();
	file       = asset;

	asset->SetFilepath(filepath);

	Assert(profile_.has_value(), "profile is not set.");
	asset->SetProfile(profile_.value());
	loader_->PushTask(asset);
	return asset;
}

//std::shared_ptr<AssetAudioBuffer> AssetCollection::LoadAudioBuffer(FileData& fileData, const std::filesystem::path& filepath) {
//	auto& [type, file] = fileData;
//	Assert(type == FileType::AudioBuffer, "different type.");
//
//	auto asset = std::make_shared<AssetAudioBuffer>();
//	file       = asset;
//
//	asset->SetFilepath(filepath);
//	AsyncAssetThreadCollection::PushTask(asset);
//	return asset;
//}

std::shared_ptr<AssetTexture> AssetCollection::TryLoadTexture(FileData& fileData, const std::filesystem::path& filepath) {
	auto& [type, file] = fileData;
	Assert(type == FileType::Texture, "different type.");

	if (std::holds_alternative<std::shared_ptr<AssetTexture>>(file)) { //!< textureが存在する場合
		return std::get<std::shared_ptr<AssetTexture>>(file);
	}

	return LoadTexture(fileData, filepath);
}

std::shared_ptr<AssetModel> AssetCollection::TryLoadModel(FileData& fileData, const std::filesystem::path& filepath) {
	auto& [type, file] = fileData;
	Assert(type == FileType::Model_Animator, "different type.");

	if (std::holds_alternative<std::pair<std::shared_ptr<AssetModel>, std::shared_ptr<AssetAnimator>>>(file)) { //!< modelまたはanimatorが存在する場合
		auto& [model, animator] = std::get<std::pair<std::shared_ptr<AssetModel>, std::shared_ptr<AssetAnimator>>>(file);

		if (model != nullptr) {
			return model;
		}
	}

	return LoadModel(fileData, filepath);
}

std::shared_ptr<AssetAnimator> AssetCollection::TryLoadAnimator(FileData& fileData, const std::filesystem::path& filepath) {
	auto& [type, file] = fileData;
	Assert(type == FileType::Model_Animator, "different type.");

	if (std::holds_alternative<std::pair<std::shared_ptr<AssetModel>, std::shared_ptr<AssetAnimator>>>(file)) { //!< modelまたはanimatorが存在する場合
		auto& [model, animator] = std::get<std::pair<std::shared_ptr<AssetModel>, std::shared_ptr<AssetAnimator>>>(file);

		if (animator != nullptr) {
			return animator;
		}
	}

	return LoadAnimator(fileData, filepath);
}

std::shared_ptr<AssetBlob> AssetCollection::TryLoadBlob(FileData& fileData, const std::filesystem::path& filepath) {
	auto& [type, file] = fileData;
	Assert(type == FileType::Shader, "different type.");

	if (std::holds_alternative<std::shared_ptr<AssetBlob>>(file)) { //!< blobが存在する場合
		return std::get<std::shared_ptr<AssetBlob>>(file);
	}

	return LoadBlob(fileData, filepath);
}

//std::shared_ptr<AssetAudioBuffer> AssetCollection::TryLoadAudioBuffer(FileData& fileData, const std::filesystem::path& filepath) {
//	auto& [type, file] = fileData;
//	Assert(type == FileType::AudioBuffer, "different type.");
//
//	if (std::holds_alternative<std::shared_ptr<AssetAudioBuffer>>(file)) { //!< audio bufferが存在する場合
//		return std::get<std::shared_ptr<AssetAudioBuffer>>(file);
//	}
//
//	return LoadAudioBuffer(fileData, filepath);
//}



