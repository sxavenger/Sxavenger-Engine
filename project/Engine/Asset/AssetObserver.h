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

	bool CheckExpired() const { return asset_.expired(); }

	std::shared_ptr<T> Lock();

	std::shared_ptr<T> ReloadAndLock();

	std::shared_ptr<T> WaitAndLock();

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
	Assert(collection_ != nullptr, "asset collection is not set.");
	Assert(!filepath_.empty(), "asset filepath is empty.");
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
	std::shared_ptr<T> asset = Lock();
	asset->WaitComplete();
	return asset;
}


