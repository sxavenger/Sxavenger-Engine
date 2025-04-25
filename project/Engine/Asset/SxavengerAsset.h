#pragma once

//-----------------------------------------------------------------------------------------
// includ
//-----------------------------------------------------------------------------------------
//* asset
#include "AssetStorage.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerAsset class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Term();

	//* import *//

	template <BaseAssetConcept T>
	static AssetObserver<T> Import(const std::filesystem::path& filepath, const std::any& param = std::any());

	template <BaseAssetConcept T>
	static AssetObserver<T> TryImport(const std::filesystem::path& filepath, const std::any& param = std::any());

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerAsset class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <BaseAssetConcept T>
inline AssetObserver<T> SxavengerAsset::Import(const std::filesystem::path& filepath, const std::any& param) {
	return sAssetStorage->Import<T>(filepath, param);
}

template <BaseAssetConcept T>
inline AssetObserver<T> SxavengerAsset::TryImport(const std::filesystem::path& filepath, const std::any& param) {
	return sAssetStorage->TryImport<T>(filepath, param);
}
