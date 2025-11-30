#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "BaseAsset.h"
#include "AssetTexture.h"
#include "AssetMesh.h"
#include "AssetMaterial.h"
#include "AssetAnimation.h"
#include "AssetSkeleton.h"
#include "AssetFont.h"
#include "AssetAudioClip.h"

//* engine
#include <Engine/System/Config/SxavengerConfig.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* c++
#include <unordered_map>
#include <memory>
#include <typeinfo>
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetStorage {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Storage = std::unordered_map<const std::type_info*, std::unordered_map<Uuid, std::shared_ptr<BaseAsset>>>;

	using IdTable = std::unordered_map<Uuid, std::pair<std::filesystem::path, std::string>>;
	
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Term() { storage_.clear(); }

	//* storage option *//

	//! @brief Assetの登録
	//! @tparam T Assetの型
	//! @param[in] asset 登録するAssetの共有ポインタ
	//! @param[in] filepath Assetのファイルパス
	template <AssetConcept T>
	void Register(const std::shared_ptr<T>& asset, const std::filesystem::path& filepath);

	//! @brief Assetの取得
	//! @tparam T Assetの型
	//! @param[in] id Assetのuuid
	//! @retval ptr     取得したAssetの共有ポインタ
	//! @retval nullptr Assetが存在しない場合
	template <AssetConcept T>
	std::shared_ptr<T> GetAsset(const Uuid& id) const;

	//! @brief Assetの全要素に対して関数を実行
	//! @tparam T Assetの型
	//! @param[in] function 実行する関数
	template <AssetConcept T>
	void ForEach(const std::function<void(T* const)>& function) const;

	//! @brief Assetの存在確認
	//! @tparam T Assetの型
	//! @param[in] id Assetのuuid
	//! @retval true  Assetが存在する場合
	//! @retval false Assetが存在しない場合
	template <AssetConcept T>
	bool Contains(const Uuid& id) const { return Contains(&typeid(T), id); }

	template <AssetConcept T>
	const std::unordered_map<Uuid, std::shared_ptr<BaseAsset>>& GetAssetStorage();

	const Storage& GetStorage() const { return storage_; }

	//* table option *//

	void Serialize() const;

	void Deserialize();

	const std::filesystem::path& GetFilepath(const Uuid& id) const;

	//* singleton *//

	static AssetStorage* GetInstance();

private:

	//=========================================================================================
	// private vaariables
	//=========================================================================================

	//* storage *//

	Storage storage_;

	//* table *//

	IdTable table_;
	static inline const std::filesystem::path kTableFilepath_ = kPackagesDirectory / "intermediate" / "asset_table.uasset";

	//=========================================================================================
	// private methods
	//=========================================================================================

	bool Contains(const std::type_info* type, const Uuid& id) const {
		return storage_.contains(type) && storage_.at(type).contains(id);
	}

	template <AssetConcept T>
	static std::shared_ptr<T> Cast(const std::shared_ptr<BaseAsset>& asset) {
		return std::static_pointer_cast<T>(asset);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetStorage class template methods
////////////////////////////////////////////////////////////////////////////////////////////
template <AssetConcept T>
inline void AssetStorage::Register(const std::shared_ptr<T>& asset, const std::filesystem::path& filepath) {
	constexpr const std::type_info* type = &typeid(T);

	const Uuid& id = asset->GetId();

	storage_[type][id] = asset;
	table_[id]         = std::make_pair(filepath, type->name());
}

template <AssetConcept T>
inline std::shared_ptr<T> AssetStorage::GetAsset(const Uuid& id) const {
	constexpr const std::type_info* type = &typeid(T);

	if (!Contains(type, id)) {
		return nullptr; //!< Assetが存在しない場合はnullptrを返す
	}
	// todo: ここをwaitにさせる...?

	return AssetStorage::Cast<T>(storage_.at(type).at(id)); //!< Assetを取得
}

template <AssetConcept T>
void AssetStorage::ForEach(const std::function<void(T* const)>& function) const {
	constexpr const std::type_info* type = &typeid(T);

	if (!storage_.contains(type)) {
		return; //!< Assetが存在しない場合は何もしない
	}

	for (const auto& [id, asset] : storage_.at(type)) {
		if (!asset->IsComplete()) {
			continue; //!< Assetが未完了の場合はスキップ
		}

		function(AssetStorage::Cast<T>(asset).get());
	}
}

template <AssetConcept T>
inline const std::unordered_map<Uuid, std::shared_ptr<BaseAsset>>& AssetStorage::GetAssetStorage() {
	constexpr const std::type_info* type = &typeid(T);
	return storage_[type]; //!< Assetのストレージを返す
}

////////////////////////////////////////////////////////////////////////////////////////////
// singleton instance variable
////////////////////////////////////////////////////////////////////////////////////////////
static AssetStorage* const sAssetStorage = AssetStorage::GetInstance();
