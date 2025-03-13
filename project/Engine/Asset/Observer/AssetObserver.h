#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../BaseAsset.h"
#include "../AssetCollection.h"

//* engine
#include <Engine/System/Utility/Logger.h>

//* c++
#include <filesystem>
#include <optional>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetObserver class
////////////////////////////////////////////////////////////////////////////////////////////
template <BaseAssetConcept T>
class AssetObserver {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Condition enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Condition : uint8_t {
		UnRegistered, //!< 未登録
		Expired,      //!< 期限切れ
		Valid         //!< 有効
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetObserver()  = default;
	~AssetObserver() = default;

	void Register(const std::shared_ptr<T>& asset, AssetCollection* collection = nullptr);

	void Reload();
	//* 監視対象のassetを再度読み込む(Import(...))

	//* getter *//

	Condition GetCondition() const;

	bool CheckRegistered() const;

	bool CheckExpired() const;

	std::shared_ptr<T> Get();
	std::shared_ptr<T> Get() const; //!< todo: 通常版と機能が違うので名前を変える

	std::shared_ptr<T> WaitGet();
	std::shared_ptr<T> WaitGet() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	AssetCollection* collection_ = nullptr;

	//* asset *//

	std::optional<std::weak_ptr<T>> asset_;

	//* parameter *//

	std::filesystem::path filepath_;
	//* 再検索のためのfilepath

};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetObserver class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <BaseAssetConcept T>
inline void AssetObserver<T>::Register(const std::shared_ptr<T>& asset, AssetCollection* collection) {
	asset_ = asset;

	std::shared_ptr<BaseAsset> base = asset;
	filepath_ = base->GetFilepath();

	if (collection != nullptr) {
		collection_ = collection;
	}
}

template <BaseAssetConcept T>
inline void AssetObserver<T>::Reload() {
	Assert(GetCondition() != Condition::UnRegistered, "asset is not registered.");
	Assert(collection_ != nullptr, "collection is not set.");
	asset_ = collection_->ImportPtr<T>(filepath_);
}

template <BaseAssetConcept T>
inline AssetObserver<T>::Condition AssetObserver<T>::GetCondition() const {
	if (!asset_.has_value()) {
		return Condition::UnRegistered;
	}

	if (asset_.value().expired()) {
		return Condition::Expired;
	}

	return Condition::Valid;
}

template<BaseAssetConcept T>
inline bool AssetObserver<T>::CheckRegistered() const {
	return GetCondition() != Condition::UnRegistered;
}

template<BaseAssetConcept T>
inline bool AssetObserver<T>::CheckExpired() const {
	Condition condition = GetCondition();
	Assert(condition != Condition::UnRegistered, "asset is not registered.");
	return condition == Condition::Expired;
}

template <BaseAssetConcept T>
inline std::shared_ptr<T> AssetObserver<T>::Get() {
	Condition condition = GetCondition();
	Assert(condition != Condition::UnRegistered, "asset is not registered.");

	if (condition == Condition::Expired) {
		Assert(collection_ != nullptr, "collection is not set.");
		asset_ = collection_->TryImportPtr<T>(filepath_);
	}

	return asset_.value().lock();
}

template <BaseAssetConcept T>
inline std::shared_ptr<T> AssetObserver<T>::Get() const {
	Condition condition = GetCondition();

	Assert(condition == Condition::Valid, "asset is not valid");

	return asset_.value().lock();
}

template <BaseAssetConcept T>
inline std::shared_ptr<T> AssetObserver<T>::WaitGet() {
	std::shared_ptr<T> asset = Get();
	asset->WaitComplete();
	return asset;
}

template <BaseAssetConcept T>
inline std::shared_ptr<T> AssetObserver<T>::WaitGet() const {
	std::shared_ptr<T> asset = Get();
	asset->WaitComplete();
	return asset;
}
