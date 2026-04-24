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

////////////////////////////////////////////////////////////////////////////////////////////
// Asset namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Asset {

	//////////////////////////////////////////////////////////////////////////////////////////
	// Storage class
	//////////////////////////////////////////////////////////////////////////////////////////
	class Storage final {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// using
		////////////////////////////////////////////////////////////////////////////////////////////

		using StorageContainer = std::unordered_map<Uuid, std::shared_ptr<BaseAsset>>;
		using StageStorage     = std::unordered_map<const std::type_info*, StorageContainer>;
		//!< 各AssetごとにUuidとAssetの共有ポインタを紐づけるストレージコンテナ.

		using Location = std::unordered_map<Uuid, std::filesystem::path>;
		//!< AssetのUuidとファイルパスを紐づけるロケーションテーブル.

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* storage option *//

		//! @brief Assetの登録
		//! @tparam T Assetの型
		//! @param[in] asset 登録するAssetの共有ポインタ
		//! @param[in] filepath Assetのファイルパス
		template <AssetDerived T>
		void Register(const std::shared_ptr<T>& asset, const std::filesystem::path& filepath);

		//! @brief Assetの存在確認
		//! @tparam T Assetの型
		//! @param[in] id Assetのuuid
		//! @retval true  Assetが存在する
		//! @retval false Assetが存在しない
		template <AssetDerived T>
		bool Contains(const Uuid& id) const { return Contains(&typeid(T), id); }

		//! @brief Assetの取得
		//! @tparam T Assetの型
		//! @param[in] id Assetのuuid
		//! @retval ptr     取得したAssetの共有ポインタ
		//! @retval nullptr Assetが存在しない場合
		template <AssetDerived T>
		std::shared_ptr<T> Get(const Uuid& id) const;


		//! @brief Assetの全要素に対して関数を実行
		//! @tparam T Assetの型
		//! @param[in] function 実行する関数
		template <AssetDerived T>
		void ForEach(const std::function<void(T* const)>& function) const;

		//* location option *//

		void SerializeLocation() const;

		void DeserializeLocation();

		const std::filesystem::path& GetLocation(const Uuid& id) const;

		//* singleton *//

		static Storage* GetInstance();

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* storage *//

		StageStorage storage_;

		//* location *//

		Location location_;

		static inline const std::filesystem::path kLocationFilepath = kPackagesDirectory / "intermediate" / "asset_location.asset";

		//=========================================================================================
		// private methods
		//=========================================================================================

		bool Contains(const std::type_info* type, const Uuid& id) const {
			return storage_.contains(type) && storage_.at(type).contains(id);
		}

		template <AssetDerived T>
		static std::shared_ptr<T> Cast(const std::shared_ptr<BaseAsset>& asset) {
			return std::static_pointer_cast<T>(asset);
		}

		template <AssetDerived T>
		static T* Cast(BaseAsset* asset) {
			return static_cast<T*>(asset);
		}

	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// Storage class template methods
	//////////////////////////////////////////////////////////////////////////////////////////

	template <AssetDerived T>
	void Storage::Register(const std::shared_ptr<T>& asset, const std::filesystem::path& filepath) {
		constexpr const std::type_info* type = &typeid(T);

		const Uuid& id = asset->GetId();

		storage_[type][id] = asset;
		location_[id]      = filepath;
	}

	template <AssetDerived T>
	std::shared_ptr<T> Storage::Get(const Uuid& id) const {
		constexpr const std::type_info* type = &typeid(T);

		if (!Contains(type, id)) {
			return nullptr; //!< Assetが存在しない場合はnullptrを返す
		}

		return Storage::Cast<T>(storage_.at(type).at(id));
	}

	template <AssetDerived T>
	void Storage::ForEach(const std::function<void(T* const)>& function) const {
		constexpr const std::type_info* type = &typeid(T);
		if (!storage_.contains(type)) {
			return; //!< Assetが存在しない場合は何もしない
		}

		for (const auto& asset : storage_.at(type) | std::views::values) {
			function(Storage::Cast<T>(asset.get()));
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// instance
	//////////////////////////////////////////////////////////////////////////////////////////

	static Storage* const sStorage = Storage::GetInstance();

}

SXAVENGER_ENGINE_NAMESPACE_END


