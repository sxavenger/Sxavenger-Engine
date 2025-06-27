#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../Assets/BaseAsset.h"
#include "../AssetStorage.h"

//* engine
#include <Engine/System/Utility/Logger.h>

//* c++
#include <filesystem>
#include <optional>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCondition enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class AssetCondition : uint8_t {
	UnRegistered, //!< 未登録
	Expired,      //!< 期限切れ
	Valid         //!< 有効
};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetObserver class
////////////////////////////////////////////////////////////////////////////////////////////
template <BaseAssetConcept T>
class AssetObserver {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* observer option *//

	void Register(const std::shared_ptr<T>& asset);

	void Hotreload();

	void Reload(); //!< todo: 関数名変更

	AssetCondition GetCondition() const;

	//* accessor *//

	bool IsRegistered() const;

	bool IsExpired() const;

	std::shared_ptr<T> Acquire();
	std::shared_ptr<T> WaitAcquire();

	std::shared_ptr<T> Get() const;
	std::shared_ptr<T> WaitGet() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::optional<std::weak_ptr<T>> asset_; //!< 監視対象のアセット

	//* parameter *//

	std::filesystem::path filepath_;
	std::any param_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	
};


////////////////////////////////////////////////////////////////////////////////////////////
// AssetObserver class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <BaseAssetConcept T>
void AssetObserver<T>::Register(const std::shared_ptr<T>& asset) {
	asset_    = asset;
	filepath_ = asset->GetFilepath();
	param_    = asset->GetParam();
}

template <BaseAssetConcept T>
inline void AssetObserver<T>::Hotreload() {
	*this = sAssetStorage->Import<T>(filepath_, param_);
}

template <BaseAssetConcept T>
inline void AssetObserver<T>::Reload() {
	*this = sAssetStorage->TryImport<T>(filepath_, param_);
}

template <BaseAssetConcept T>
inline AssetCondition AssetObserver<T>::GetCondition() const {
	if (!asset_.has_value()) {
		return AssetCondition::UnRegistered;
	}

	if (asset_.value().expired()) {
		return AssetCondition::Expired;
	}

	return AssetCondition::Valid;
}

template <BaseAssetConcept T>
inline bool AssetObserver<T>::IsRegistered() const {
	return GetCondition() != AssetCondition::UnRegistered;
}

template <BaseAssetConcept T>
inline bool AssetObserver<T>::IsExpired() const {
	AssetCondition condition = GetCondition();
	Exception::Assert(condition != AssetCondition::UnRegistered, "asset is not registered.");
	return condition == AssetCondition::Expired;
}

template <BaseAssetConcept T>
inline std::shared_ptr<T> AssetObserver<T>::Acquire() {
	AssetCondition condition = GetCondition();
	Exception::Assert(condition != AssetCondition::UnRegistered, "asset is not registered.");

	if (condition == AssetCondition::Expired) {
		Reload();
	}

	return (*asset_).lock();
}

template <BaseAssetConcept T>
inline std::shared_ptr<T> AssetObserver<T>::WaitAcquire() {
	std::shared_ptr<T> asset = Acquire();
	asset->WaitComplete();
	return asset;
}

template <BaseAssetConcept T>
inline std::shared_ptr<T> AssetObserver<T>::Get() const {
	AssetCondition condition = GetCondition();
	Exception::Assert(condition != AssetCondition::UnRegistered, "asset is not registered.");
	Exception::Assert(condition != AssetCondition::Expired,      "asset is expired.");

	return (*asset_).lock();
}

template <BaseAssetConcept T>
inline std::shared_ptr<T> AssetObserver<T>::WaitGet() const {
	std::shared_ptr<T> asset = Get();
	asset->WaitComplete();
	return asset;
}

