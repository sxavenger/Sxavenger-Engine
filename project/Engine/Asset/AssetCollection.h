#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "BaseAsset.h"
#include "Unknown/AssetUnknown.h"
#include "Texture/Texture.h"
#include "ModelAnimator/ModelAnimator.h"
#include "Blob/AssetBlob.h"
#include "Thread/AsyncAssetThreadCollection.h"
#include "AssetObserver.h"

//* engine
#include <Engine/System/Utility/Logger.h>

//* lib
#include <Lib/Sxl/OptimizedLowerPathMap.h>

//* c++
#include <filesystem>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetCollection
	: public AsyncAssetThreadCollection {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// FileType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class FileType {
		Unknown,
		Texture,
		ModelAnimator,
		Shader,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// File using
	////////////////////////////////////////////////////////////////////////////////////////////
	using File = std::pair<FileType, std::shared_ptr<BaseAsset>>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Folder structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Folder {

		//* member *//

		Sxl::OptimizedLowerPathMap<File>                                     files;
		Sxl::OptimizedLowerPathMap<std::pair<std::filesystem::path, Folder>> folder;

		//* methods *//

		bool Empty() const { return files.Empty() && folder.Empty(); }

	};
	using FolderPair = std::pair<std::filesystem::path, Folder>;


public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetCollection()  = default;
	~AssetCollection() = default;

	void Init();

	//* asset option *//
	//* import *//

	//!< default
	template <DerivedFromBaseAssetConcept T> requires (!ModelAnimatorConcept<T>)
	std::shared_ptr<T> ImportPtr(const std::filesystem::path& filepath);

	//!< model and animator
	template <ModelAnimatorConcept T>
	std::shared_ptr<T> ImportPtr(const std::filesystem::path& filepath);

	//!< base asset
	template <>
	std::shared_ptr<BaseAsset> ImportPtr(const std::filesystem::path& filepath);

	//!< observer
	template <BaseAssetConcept T>
	AssetObserver<T> Import(const std::filesystem::path& filepath);

	//* try import *//

	//!< default
	template <DerivedFromBaseAssetConcept T> requires (!ModelAnimatorConcept<T>)
	std::shared_ptr<T> TryImportPtr(const std::filesystem::path& filepath);

	//!< model and animator
	template <ModelAnimatorConcept T>
	std::shared_ptr<T> TryImportPtr(const std::filesystem::path& filepath);

	//!< base asset
	template <>
	std::shared_ptr<BaseAsset> TryImportPtr(const std::filesystem::path& filepath);

	//!< observer
	template <BaseAssetConcept T>
	AssetObserver<T> TryImport(const std::filesystem::path& filepath);

	//* get *//

	//!< default
	template <DerivedFromBaseAssetConcept T> requires (!ModelAnimatorConcept<T>)
	std::shared_ptr<T> GetAssetPtr(const std::filesystem::path& filepath);

	//!< model and animator
	template <ModelAnimatorConcept T>
	std::shared_ptr<T> GetAssetPtr(const std::filesystem::path& filepath);

	//!< base asset
	template <>
	std::shared_ptr<BaseAsset> GetAssetPtr(const std::filesystem::path& filepath);

	//!< observer
	template <BaseAssetConcept T>
	AssetObserver<T> GetAsset(const std::filesystem::path& filepath);

	//* debug *//

	void ShowAssetTree();

	//* getter *//

	FolderPair* GetRoot() { return &root_; }

	FolderPair* GetFolder(const std::filesystem::path& folderpath);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* assets *//

	FolderPair root_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	static std::filesystem::path ToLower(const std::filesystem::path& path);

	static FileType GetFileType(const std::filesystem::path& filepath);

	static File CreateFileData(const std::filesystem::path& filepath);

	template <DerivedFromBaseAssetConcept T>
	static std::shared_ptr<T> ConvertAsset(const std::shared_ptr<BaseAsset>& base);

	const File& ImportFile(const std::filesystem::path& filepath);
	const File& TryImportFile(const std::filesystem::path& filepath);

	const File& GetFile(const std::filesystem::path& filepath) const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCollection class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <DerivedFromBaseAssetConcept T> requires (!ModelAnimatorConcept<T>)
inline std::shared_ptr<T> AssetCollection::ImportPtr(const std::filesystem::path& filepath) {
	return ConvertAsset<T>(ImportFile(filepath).second);
}

template <ModelAnimatorConcept T>
inline std::shared_ptr<T> AssetCollection::ImportPtr(const std::filesystem::path& filepath) {
	auto asset = ConvertAsset<ModelAnimator>(ImportFile(filepath).second);
	asset->Create<T>();
	return asset->GetPtr<T>();
}

template <>
inline std::shared_ptr<BaseAsset> AssetCollection::ImportPtr(const std::filesystem::path& filepath) {
	return ImportFile(filepath).second;
}

template <BaseAssetConcept T>
inline AssetObserver<T> AssetCollection::Import(const std::filesystem::path& filepath) {
	AssetObserver<T> observer = {};
	observer.Create(ImportPtr<T>(filepath), filepath, this);
	return observer;
}

template <DerivedFromBaseAssetConcept T> requires (!ModelAnimatorConcept<T>)
inline std::shared_ptr<T> AssetCollection::TryImportPtr(const std::filesystem::path& filepath) {
	return ConvertAsset<T>(TryImportFile(filepath).second);
}

template <ModelAnimatorConcept T>
inline std::shared_ptr<T> AssetCollection::TryImportPtr(const std::filesystem::path& filepath) {
	auto asset = ConvertAsset<ModelAnimator>(TryImportFile(filepath).second);
	asset->TryCreate<T>();
	return asset->GetPtr<T>();
}

template <BaseAssetConcept T>
inline AssetObserver<T> AssetCollection::TryImport(const std::filesystem::path& filepath) {
	AssetObserver<T> observer = {};
	observer.Create(TryImportPtr<T>(filepath), filepath, this);
	return observer;
}

template <>
inline std::shared_ptr<BaseAsset> AssetCollection::TryImportPtr(const std::filesystem::path& filepath) {
	return TryImportFile(filepath).second;
}

template <DerivedFromBaseAssetConcept T>
inline std::shared_ptr<T> AssetCollection::ConvertAsset(const std::shared_ptr<BaseAsset>& base) {
	std::shared_ptr<T> result = std::dynamic_pointer_cast<T>(base);
	Assert(result != nullptr, "base asset type is different.");

	return result;
}

template <DerivedFromBaseAssetConcept T> requires (!ModelAnimatorConcept<T>)
inline std::shared_ptr<T> AssetCollection::GetAssetPtr(const std::filesystem::path& filepath) {
	return ConvertAsset<T>(GetFile(filepath).second);
}

template <ModelAnimatorConcept T>
inline std::shared_ptr<T> AssetCollection::GetAssetPtr(const std::filesystem::path& filepath) {
	auto asset = ConvertAsset<ModelAnimator>(GetFile(filepath).second);
	return asset->GetPtr<T>();
}

template <>
inline std::shared_ptr<BaseAsset> AssetCollection::GetAssetPtr(const std::filesystem::path& filepath) {
	return GetFile(filepath).second;
}

template <BaseAssetConcept T>
inline AssetObserver<T> AssetCollection::GetAsset(const std::filesystem::path& filepath) {
	AssetObserver<T> observer = {};
	observer.Create(GetAssetPtr<T>(filepath), filepath, this);
	return observer;
}
