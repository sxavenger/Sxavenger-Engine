#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "BaseAsset.h"
#include "AssetTexture.h"
#include "AssetMesh.h"
#include "AssetMaterial.h"
#include "AssetSkeleton.h"
#include "AssetFont.h"
#include "AssetAnimationClip.h"
#include "AssetAudioClip.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
// AssetStorage class
//////////////////////////////////////////////////////////////////////////////////////////
class AssetStorage final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Storage = std::unordered_map<Uuid, std::shared_ptr<BaseAsset>>;
	using Stage   = std::unordered_map<const std::type_info*, Storage>;
	//!< 各AssetごとにUuidとAssetの共有ポインタを紐づけるストレージコンテナ.

	using Location = std::unordered_map<Uuid, std::filesystem::path>;
	//!< AssetのUuidとファイルパスを紐づけるロケーションテーブル.

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Term();

	//* storage option *//

	//! @brief Assetの登録
	//! @tparam T Assetの型
	//! @param[in] asset 登録するAssetの共有ポインタ
	//! @param[in] filepath Assetのファイルパス
	template <Asset T>
	void Register(const std::shared_ptr<T>& asset, const std::filesystem::path& filepath);

	//! @brief Assetの登録
	//! @tparam T Assetの型
	//! @param[in] uuid Assetのid
	//! @param[in] filepath Assetのファイルパス
	template <Asset T>
	void Register(const Uuid& uuid, const std::filesystem::path& filepath);

	//! @brief Assetの存在確認
	//! @tparam T Assetの型
	//! @param[in] id Assetのuuid
	//! @retval true  Assetが存在する
	//! @retval false Assetが存在しない
	template <Asset T>
	bool Contains(const Uuid& id) const { return Contains(&typeid(T), id); }

	//! @brief Assetの取得
	//! @tparam T Assetの型
	//! @param[in] id Assetのuuid
	//! @retval ptr     取得したAssetの共有ポインタ
	//! @retval nullptr Assetが存在しない場合
	template <Asset T>
	std::shared_ptr<T> Get(const Uuid& id) const;

	template <Asset T>
	std::shared_ptr<T> Require(const Uuid& id) const;

	//! @brief Assetの全要素に対して関数を実行
	//! @tparam T Assetの型
	//! @param[in] function 実行する関数
	template <Asset T>
	void ForEach(const std::function<void(T* const)>& function) const;

	template <Asset T>
	const Storage& GetStorage();

	const Stage& GetStage() const { return storage_; }

	//* location option *//

	void SerializeLocation() const;

	void DeserializeLocation();

	const std::filesystem::path& GetLocation(const Uuid& id) const;

	//* singleton *//

	static AssetStorage* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* storage *//

	Stage storage_;

	//* location *//

	Location location_;

	static inline const std::filesystem::path kLocationFilepath = kPackagesDirectory / "intermediate" / "asset_location.asset";

	//=========================================================================================
	// private methods
	//=========================================================================================

	bool Contains(const std::type_info* type, const Uuid& id) const {
		return storage_.contains(type) && storage_.at(type).contains(id);
	}

	template <Asset T>
	static std::shared_ptr<T> Cast(const std::shared_ptr<BaseAsset>& asset) {
		std::shared_ptr<T> pointer = std::static_pointer_cast<T>(asset);
		StreamLogger::AssertA(pointer != nullptr, "[AssetStorage] failed to cast asset.");
		return pointer;
	}

	template <Asset T>
	static T* Cast(BaseAsset* asset) {
		return static_cast<T*>(asset);
	}

};

//////////////////////////////////////////////////////////////////////////////////////////
// AssetStorage class template methods
//////////////////////////////////////////////////////////////////////////////////////////

template <Asset T>
void AssetStorage::Register(const std::shared_ptr<T>& asset, const std::filesystem::path& filepath) {
	constexpr const std::type_info* type = &typeid(T);

	const Uuid& id = asset->GetId();

	storage_[type][id] = asset;
	location_[id]      = filepath;
}

template <Asset T>
void AssetStorage::Register(const Uuid& uuid, const std::filesystem::path& filepath) {
	AssetStorage::Register<T>(std::make_shared<T>(uuid), filepath);
}

template <Asset T>
std::shared_ptr<T> AssetStorage::Get(const Uuid& id) const {
	constexpr const std::type_info* type = &typeid(T);

	if (!Contains(type, id)) {
		return nullptr; //!< Assetが存在しない場合はnullptrを返す
	}

	return AssetStorage::Cast<T>(storage_.at(type).at(id));
}

template<Asset T>
inline std::shared_ptr<T> AssetStorage::Require(const Uuid& id) const {
	std::shared_ptr<T> asset = Get<T>(id);
	StreamLogger::AssertA(asset != nullptr, "[AssetStorage] asset not found. uuid: " + id.Serialize());
	return asset;
}

template <Asset T>
void AssetStorage::ForEach(const std::function<void(T* const)>& function) const {
	constexpr const std::type_info* type = &typeid(T);
	if (!storage_.contains(type)) {
		return; //!< Assetが存在しない場合は何もしない
	}

	for (const auto& asset : storage_.at(type) | std::views::values) {
		function(AssetStorage::Cast<T>(asset.get()));
	}
}

template <Asset T>
inline const AssetStorage::Storage& AssetStorage::GetStorage() {
	constexpr const std::type_info* type = &typeid(T);
	return storage_[type];
}

//////////////////////////////////////////////////////////////////////////////////////////
// instance
//////////////////////////////////////////////////////////////////////////////////////////

static AssetStorage* const sAssetStorage = AssetStorage::GetInstance();

SXAVENGER_ENGINE_NAMESPACE_END


