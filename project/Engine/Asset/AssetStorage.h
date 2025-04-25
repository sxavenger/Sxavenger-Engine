#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* assets
#include "Assets/BaseAsset.h"
#include "Assets/Texture/AssetTexture.h"
#include "Assets/Model/AssetModel.h"
#include "Assets/Animator/AssetAnimator.h"
#include "Assets/Blob/AssetBlob.h"

//* lib
#include <Lib/Sxl/OptimizedLowerPathMap.h>

//* c++
#include <filesystem>
#include <unordered_map>
#include <typeinfo>

////////////////////////////////////////////////////////////////////////////////////////////
// forward
////////////////////////////////////////////////////////////////////////////////////////////
template <BaseAssetConcept T>
class AssetObserver;

////////////////////////////////////////////////////////////////////////////////////////////
// AssetStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetStorage {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	template <BaseAssetConcept _Ty>
	void Import(const std::filesystem::path& filepath);


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_map<const std::type_info*, Sxl::OptimizedLowerPathMap<std::shared_ptr<BaseAsset>>> storage_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetStorage class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <BaseAssetConcept _Ty>
void AssetStorage::Import(const std::filesystem::path& filepath) {
	constexpr const std::type_info* type = &typeid(_Ty);

	if () {

	}
}
