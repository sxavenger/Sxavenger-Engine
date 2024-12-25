#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "BaseAsset.h"

//* c++
#include <memory>
#include <filesystem>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class AssetCollection;

////////////////////////////////////////////////////////////////////////////////////////////
// AssetObserver class
////////////////////////////////////////////////////////////////////////////////////////////
template <BaseAssetConcept T>
class AssetObserver {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetObserver()  = default;
	~AssetObserver() = default;

	void Create(const std::shared_ptr<T>& asset, const std::filesystem::path& filepath, AssetCollection* collection);

	void Reload();

	void CheckAndReload();

	//* getter *//

	bool Check() const { return !asset_.expired(); }

	std::shared_ptr<T> Lock();

	std::shared_ptr<T> ReloadAndLock();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	AssetCollection* collection_ = nullptr;

	//* asset *//

	std::weak_ptr<T> asset_;

	//* filepath *//

	std::filesystem::path filepath_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetObserver class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <BaseAssetConcept T>
inline void AssetObserver<T>::Create(const std::shared_ptr<T>& asset, const std::filesystem::path& filepath, AssetCollection* collection) {
	asset_      = asset;
	filepath_   = filepath;
	collection_ = collection;
}

template<BaseAssetConcept T>
inline void AssetObserver<T>::Reload() {
	if (filepath_.empty()) {
		return;
	}

	Assert(collection_ != nullptr, "asset collection is not set.");
	asset_ = collection_->template GetAssetPtr<T>(filepath_);
}

template<BaseAssetConcept T>
inline void AssetObserver<T>::CheckAndReload() {
	if (!Check()) {
		Reload();
	}
}

template <BaseAssetConcept T>
inline std::shared_ptr<T> AssetObserver<T>::Lock() {
	Assert(Check(), "asset is expired.");
	return asset_.lock();
}

template<BaseAssetConcept T>
inline std::shared_ptr<T> AssetObserver<T>::ReloadAndLock() {
	CheckAndReload();
	return Lock();
}


