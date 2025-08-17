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

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* c++
#include <unordered_map>
#include <memory>
#include <typeinfo>

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

	//* storage option *//

	template <UAssetConcept T>
	void Register(const std::shared_ptr<T>& asset, const std::filesystem::path& filepath);

	template <UAssetConcept T>
	std::shared_ptr<T> GetAsset(const Uuid& id) const;

	//* table option *//

	void Serialize() const;

	void Deserialize();

	const std::filesystem::path& GetFilepath(const Uuid& id) const;

	//* singleton *//

	static UAssetStorage* GetInstance();

private:

	//=========================================================================================
	// public vaariables
	//=========================================================================================

	//* storage *//

	Storage storage_;

	//* table *//

	IdTable table_;
	static inline const std::filesystem::path kTableFilepath_ = "asset_table.uasset";

	//=========================================================================================
	// public methods
	//=========================================================================================

	bool Contains(const std::type_info* type, const Uuid& id) const {
		return storage_.contains(type) && storage_.at(type).contains(id);
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

	return std::static_pointer_cast<T>(storage_.at(type).at(id)); //!< Assetを取得
}

////////////////////////////////////////////////////////////////////////////////////////////
// singleton instance variable
////////////////////////////////////////////////////////////////////////////////////////////
static UAssetStorage* const sUAssetStorage = UAssetStorage::GetInstance();
