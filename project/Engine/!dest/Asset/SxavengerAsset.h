#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "AssetCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerAsset class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void Term();

	template <BaseAssetConcept T>
	static std::weak_ptr<T> ImportPtr(const std::filesystem::path& filepath);

	template <BaseAssetConcept T>
	static AssetObserver<T> Import(const std::filesystem::path& filepath);

	template <BaseAssetConcept T>
	static std::weak_ptr<T> TryImportPtr(const std::filesystem::path& filepath);

	template <BaseAssetConcept T>
	static AssetObserver<T> TryImport(const std::filesystem::path& filepath);

	template <BaseAssetConcept T>
	static std::shared_ptr<T> GetAssetPtr(const std::filesystem::path& filepath);

	template <BaseAssetConcept T>
	static AssetObserver<T> GetAsset(const std::filesystem::path& filepath);

	static void PushTask(const std::shared_ptr<BaseAsset>& asset);

	static AssetCollection* GetCollection() { return collection_.get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static std::unique_ptr<AssetCollection> collection_;
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerAsset class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <BaseAssetConcept T>
inline std::weak_ptr<T> SxavengerAsset::ImportPtr(const std::filesystem::path& filepath) {
	return collection_->ImportPtr<T>(filepath);
}

template<BaseAssetConcept T>
inline AssetObserver<T> SxavengerAsset::Import(const std::filesystem::path& filepath) {
	return collection_->Import<T>(filepath);
}

template<BaseAssetConcept T>
inline std::weak_ptr<T> SxavengerAsset::TryImportPtr(const std::filesystem::path& filepath) {
	return collection_->TryImportPtr<T>(filepath);
}

template <BaseAssetConcept T>
inline AssetObserver<T> SxavengerAsset::TryImport(const std::filesystem::path& filepath) {
	return collection_->TryImport<T>(filepath);
}

template<BaseAssetConcept T>
inline std::shared_ptr<T> SxavengerAsset::GetAssetPtr(const std::filesystem::path& filepath) {
	return collection_->GetAssetPtr<T>(filepath);
}

template <BaseAssetConcept T>
inline AssetObserver<T> SxavengerAsset::GetAsset(const std::filesystem::path& filepath) {
	return collection_->GetAsset<T>(filepath);
}
