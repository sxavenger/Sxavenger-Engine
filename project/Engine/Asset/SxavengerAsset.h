#pragma once

//-----------------------------------------------------------------------------------------
// includ
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

	//* import *//

	template <BaseAssetConcept T>
	static AssetObserver<T> Import(const std::filesystem::path& filepath);

	template <BaseAssetConcept T>
	static std::shared_ptr<T> ImportPtr(const std::filesystem::path& filepath);

	//* try import *//

	template <BaseAssetConcept T>
	static AssetObserver<T> TryImport(const std::filesystem::path& filepath);

	//* option *//

	static void SetNextCompileProfile(DxObject::CompileProfile profile) { sAssetCollection->SetCompileProfile(profile); }

	static void SetNextAssimpOptionModel(uint32_t option) { sAssetCollection->SetAssimpOptionModel(option); }

	static void SetNextAssimpOptionAnimator(uint32_t option) { sAssetCollection->SetAssimpOptionAnimator(option); }

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerAsset class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <BaseAssetConcept T>
inline AssetObserver<T> SxavengerAsset::Import(const std::filesystem::path& filepath) {
	return sAssetCollection->Import<T>(filepath);
}

template <BaseAssetConcept T>
inline std::shared_ptr<T> SxavengerAsset::ImportPtr(const std::filesystem::path& filepath) {
	return sAssetCollection->ImportPtr<T>(filepath);
}

template <BaseAssetConcept T>
inline AssetObserver<T> SxavengerAsset::TryImport(const std::filesystem::path& filepath) {
	return sAssetCollection->TryImport<T>(filepath);
}
