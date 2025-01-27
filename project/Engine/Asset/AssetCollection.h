#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "Unknown/AssetUnknown.h"
#include "Texture/AssetTexture.h"
#include "Model/AssetModel.h"
#include "Animator/AssetAnimator.h"
#include "Blob/AssetBlob.h"
#include "AudioBuffer/AssetAudioBuffer.h"

//* other asset adapter
#include "Thread/AsyncAssetThread.h"
//#include "Observer/AssetObserver.h"

//* lib
#include <Lib/Sxl/OptimizedLowerPathMap.h>

//* c++
#include <filesystem>
#include <memory>
#include <variant>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
template <BaseAssetConcept T>
class AssetObserver;

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetCollection
	: public AsyncAssetThreadCollection {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// FileType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class FileType {
		Unknown,
		Texture,
		Model_Animator,
		Shader,
		AudioBuffer,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// File variant
	////////////////////////////////////////////////////////////////////////////////////////////
	using File = std::variant<
		std::monostate,
		std::shared_ptr<AssetUnknown>,
		std::shared_ptr<AssetTexture>,
		std::pair<std::shared_ptr<AssetModel>, std::shared_ptr<AssetAnimator>>, // todo: usingでまとめる
		std::shared_ptr<AssetBlob>,
		std::shared_ptr<AssetAudioBuffer>
	>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// FileData using
	////////////////////////////////////////////////////////////////////////////////////////////
	using FileData = std::pair<FileType, File>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// FolderData using
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Folder;
	using FolderData = std::pair<std::filesystem::path, Folder>;
	//* first:  rootからの相対パス
	//* second: Folder

	////////////////////////////////////////////////////////////////////////////////////////////
	// Folder structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Folder {

		//* member *//

		Sxl::OptimizedLowerPathMap<FileData>   files;
		Sxl::OptimizedLowerPathMap<FolderData> folders;

		//* methods *//

		bool IsEmpty() const { return files.Empty() && folders.Empty(); };

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetCollection()  = default;
	~AssetCollection() { Term(); }

	void Init();

	void Term();

	//* import *//

	template <BaseAssetConcept T>
	AssetObserver<T> Import(const std::filesystem::path& filepath);

	template <BaseAssetConcept T>
	std::shared_ptr<T> ImportPtr(const std::filesystem::path& filepath);

	//* try import *//

	template <BaseAssetConcept T>
	AssetObserver<T> TryImport(const std::filesystem::path& filepath);

	template <BaseAssetConcept T>
	std::shared_ptr<T> TryImportPtr(const std::filesystem::path& filepath);

	//* import option *//

	void SetAssimpOptionModel(uint32_t option) { assimpOptionModel_ = option; }

	void SetAssimpOptionAnimator(uint32_t option) { assimpOptionAnimator_ = option; }

	void SetCompileProfile(const DxObject::CompileProfile& profile) { profile_ = profile; }

	//* getter *//

	const FileData& GetFile(const std::filesystem::path& filepath) const;
	FileData& GetFile(const std::filesystem::path& filepath);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* assets *//

	FolderData root_ = {};

	//* next load option *//

	uint32_t assimpOptionModel_    = Model::GetDefaultAssimpOption(); //!< model
	uint32_t assimpOptionAnimator_ = 0;  //!< animator

	std::optional<DxObject::CompileProfile> profile_ = std::nullopt; //!< blob

	//=========================================================================================
	// private methods
	//=========================================================================================

	static FileType GetFileType(const std::filesystem::path& filepath);

	static FileData AllocateFileType(const std::filesystem::path& filepath);

	static std::filesystem::path ToLower(const std::filesystem::path& path);

	template <BaseAssetConcept T>
	std::shared_ptr<T> ConvertAsset(const std::shared_ptr<BaseAsset>& base);

	//* load *//

	std::shared_ptr<AssetUnknown> LoadUnknown(FileData& fileData, const std::filesystem::path& filepath);
	std::shared_ptr<AssetTexture> LoadTexture(FileData& fileData, const std::filesystem::path& filepath);
	std::shared_ptr<AssetModel> LoadModel(FileData& fileData, const std::filesystem::path& filepath);
	std::shared_ptr<AssetAnimator> LoadAnimator(FileData& fileData, const std::filesystem::path& filepath);
	std::shared_ptr<AssetBlob> LoadBlob(FileData& fileData, const std::filesystem::path& filepath);
	std::shared_ptr<AssetAudioBuffer> LoadAudioBuffer(FileData& fileData, const std::filesystem::path& filepath);

	//* try load *//

	std::shared_ptr<AssetUnknown> TryLoadUnknown(FileData& fileData, const std::filesystem::path& filepath);
	std::shared_ptr<AssetTexture> TryLoadTexture(FileData& fileData, const std::filesystem::path& filepath);
	std::shared_ptr<AssetModel> TryLoadModel(FileData& fileData, const std::filesystem::path& filepath);
	std::shared_ptr<AssetAnimator> TryLoadAnimator(FileData& fileData, const std::filesystem::path& filepath);
	std::shared_ptr<AssetBlob> TryLoadBlob(FileData& fileData, const std::filesystem::path& filepath);
	std::shared_ptr<AssetAudioBuffer> TryLoadAudioBuffer(FileData& fileData, const std::filesystem::path& filepath);

};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCollection class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <BaseAssetConcept T>
inline AssetObserver<T> AssetCollection::Import(const std::filesystem::path& filepath) {

	AssetObserver<T> observer;

	auto& fileData = GetFile(filepath);

	if constexpr (std::is_same_v<T, AssetUnknown>) {
		observer.Register(LoadUnknown(fileData, filepath), this);

	} else if constexpr (std::is_same_v<T, AssetTexture>) {
		observer.Register(LoadTexture(fileData, filepath), this);

	} else if constexpr (std::is_same_v<T, AssetModel>) {
		observer.Register(LoadModel(fileData, filepath), this);

	} else if constexpr (std::is_same_v<T, AssetAnimator>) {
		observer.Register(LoadAnimator(fileData, filepath), this);

	} else if constexpr (std::is_same_v<T, AssetBlob>) {
		observer.Register(LoadBlob(fileData, filepath), this);

	} else if constexpr (std::is_same_v<T, AssetAudioBuffer>) {
		observer.Register(LoadAudioBuffer(fileData, filepath), this);

	} else {
		static_assert(false, "not supported type.");
	}

	return observer;
}

template<BaseAssetConcept T>
inline std::shared_ptr<T> AssetCollection::ImportPtr(const std::filesystem::path& filepath) {
	auto& fileData = GetFile(filepath);

	if constexpr (std::is_same_v<T, AssetUnknown>) {
		return LoadUnknown(fileData, filepath);

	} else if constexpr (std::is_same_v<T, AssetTexture>) {
		return LoadTexture(fileData, filepath);

	} else if constexpr (std::is_same_v<T, AssetModel>) {
		return LoadModel(fileData, filepath);

	} else if constexpr (std::is_same_v<T, AssetAnimator>) {
		return LoadAnimator(fileData, filepath);

	} else if constexpr (std::is_same_v<T, AssetBlob>) {
		return LoadBlob(fileData, filepath);

	} else if constexpr (std::is_same_v<T, AssetAudioBuffer>) {
		return LoadAudioBuffer(fileData, filepath);

	} else {
		static_assert(false, "not supported type.");
	}
}

template<BaseAssetConcept T>
inline AssetObserver<T> AssetCollection::TryImport(const std::filesystem::path& filepath) {

	AssetObserver<T> observer;

	auto& fileData = GetFile(filepath);

	if constexpr (std::is_same_v<T, AssetUnknown>) {
		observer.Register(TryLoadUnknown(fileData, filepath), this);

	} else if constexpr (std::is_same_v<T, AssetTexture>) {
		observer.Register(TryLoadTexture(fileData, filepath), this);

	} else if constexpr (std::is_same_v<T, AssetModel>) {
		observer.Register(TryLoadModel(fileData, filepath), this);

	} else if constexpr (std::is_same_v<T, AssetAnimator>) {
		observer.Register(TryLoadAnimator(fileData, filepath), this);

	} else if constexpr (std::is_same_v<T, AssetBlob>) {
		observer.Register(TryLoadBlob(fileData, filepath), this);

	} else if constexpr (std::is_same_v<T, AssetAudioBuffer>) {
		observer.Register(TryLoadAudioBuffer(fileData, filepath), this);

	} else {
		static_assert(false, "not supported type.");
	}

	return observer;
}

template <BaseAssetConcept T>
inline std::shared_ptr<T> AssetCollection::TryImportPtr(const std::filesystem::path& filepath) {
	auto& fileData = GetFile(filepath);

	if constexpr (std::is_same_v<T, AssetUnknown>) {
		return TryLoadUnknown(fileData, filepath);

	} else if constexpr (std::is_same_v<T, AssetTexture>) {
		return TryLoadTexture(fileData, filepath);

	} else if constexpr (std::is_same_v<T, AssetModel>) {
		return TryLoadModel(fileData, filepath);

	} else if constexpr (std::is_same_v<T, AssetAnimator>) {
		return TryLoadAnimator(fileData, filepath);

	} else if constexpr (std::is_same_v<T, AssetBlob>) {
		return TryLoadBlob(fileData, filepath);

	} else if constexpr (std::is_same_v<T, AssetAudioBuffer>) {
		return TryLoadAudioBuffer(fileData, filepath);

	} else {
		static_assert(false, "not supported type.");
	}
}

template <BaseAssetConcept T>
inline std::shared_ptr<T> AssetCollection::ConvertAsset(const std::shared_ptr<BaseAsset>& base) {
	auto asset = std::dynamic_pointer_cast<T>(base);
	Assert(asset, "asset is not type.");
	return asset;
}
