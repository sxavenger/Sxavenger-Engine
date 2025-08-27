#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "UBaseAsset.h"
#include "UAssetTexture.h"
#include "UAssetMesh.h"
#include "UAssetMaterial.h"
#include "UAssetAnimation.h"
#include "UAssetSkeleton.h"

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
// UAssetStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class UAssetStorage {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Storage = std::unordered_map<const std::type_info*, std::unordered_map<Uuid, std::shared_ptr<UBaseAsset>>>;

	using IdTable = std::unordered_map<Uuid, std::pair<std::filesystem::path, std::string>>;
	
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Term() { storage_.clear(); }

	//* storage option *//

	template <UAssetConcept T>
	void Register(const std::shared_ptr<T>& asset, const std::filesystem::path& filepath);

	template <UAssetConcept T>
	std::shared_ptr<T> GetAsset(const Uuid& id) const;

	template <UAssetConcept T>
	void ForEach(const std::function<void(T* const)>& function) const;

	template <UAssetConcept T>
	bool Contains(const Uuid& id) const { return Contains(&typeid(T), id); }

	template <UAssetConcept T>
	const std::unordered_map<Uuid, std::shared_ptr<UBaseAsset>>& GetAssetStorage();

	//* table option *//

	void Serialize() const;

	void Deserialize();

	const std::filesystem::path& GetFilepath(const Uuid& id) const;

	//* singleton *//

	static UAssetStorage* GetInstance();

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

	template <UAssetConcept T>
	static std::shared_ptr<T> Cast(const std::shared_ptr<UBaseAsset>& asset) {
		return std::static_pointer_cast<T>(asset);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetStorage class template methods
////////////////////////////////////////////////////////////////////////////////////////////
template <UAssetConcept T>
inline void UAssetStorage::Register(const std::shared_ptr<T>& asset, const std::filesystem::path& filepath) {
	constexpr const std::type_info* type = &typeid(T);

	const Uuid& id = asset->GetId();

	storage_[type][id] = asset;
	table_[id]         = std::make_pair(filepath, type->name());
}

template <UAssetConcept T>
inline std::shared_ptr<T> UAssetStorage::GetAsset(const Uuid& id) const {
	constexpr const std::type_info* type = &typeid(T);

	if (!Contains(type, id)) {
		return nullptr; //!< Assetが存在しない場合はnullptrを返す
	}
	// todo: ここをwaitにさせる...?

	return UAssetStorage::Cast<T>(storage_.at(type).at(id)); //!< Assetを取得
}

template <UAssetConcept T>
void UAssetStorage::ForEach(const std::function<void(T* const)>& function) const {
	constexpr const std::type_info* type = &typeid(T);

	if (!storage_.contains(type)) {
		return; //!< Assetが存在しない場合は何もしない
	}

	for (const auto& [id, asset] : storage_.at(type)) {
		if (!asset->IsComplete()) {
			continue; //!< Assetが未完了の場合はスキップ
		}

		function(UAssetStorage::Cast<T>(asset).get());
	}
}

template<UAssetConcept T>
inline const std::unordered_map<Uuid, std::shared_ptr<UBaseAsset>>& UAssetStorage::GetAssetStorage() {
	constexpr const std::type_info* type = &typeid(T);
	return storage_[type]; //!< Assetのストレージを返す
}

////////////////////////////////////////////////////////////////////////////////////////////
// singleton instance variable
////////////////////////////////////////////////////////////////////////////////////////////
static UAssetStorage* const sUAssetStorage = UAssetStorage::GetInstance();
