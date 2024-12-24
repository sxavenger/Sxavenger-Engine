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
	static std::weak_ptr<T> Import(const std::filesystem::path& filepath);

	template <BaseAssetConcept T>
	static std::shared_ptr<T> GetAsset(const std::filesystem::path& filepath);

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
inline std::weak_ptr<T> SxavengerAsset::Import(const std::filesystem::path& filepath) {
	return collection_->Import<T>(filepath);
}

template<BaseAssetConcept T>
inline std::shared_ptr<T> SxavengerAsset::GetAsset(const std::filesystem::path& filepath) {
	return collection_->GetAsset<T>(filepath);
}
