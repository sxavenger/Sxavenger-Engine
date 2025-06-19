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

//* engine
#include <Engine/System/SxavengerSystem.h>

//* lib
#include <Lib/Sxl/OptimizedPathMap.h>

//* c++
#include <filesystem>
#include <unordered_map>
#include <typeinfo>

////////////////////////////////////////////////////////////////////////////////////////////
// forward
////////////////////////////////////////////////////////////////////////////////////////////
template <BaseAssetConcept>
class AssetObserver;

////////////////////////////////////////////////////////////////////////////////////////////
// AssetStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetStorage {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Storage = std::unordered_map<const std::type_info*, Sxl::OptimizedPathMap<std::shared_ptr<BaseAsset>>>;

	using Extensions = std::unordered_map<std::filesystem::path, std::pair<const std::type_info*, std::function<std::shared_ptr<BaseAsset>()>>>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Term();

	//* storage option *//

	template <BaseAssetConcept _Ty>
	AssetObserver<_Ty> Import(const std::filesystem::path& filepath, const std::any& param = std::any());

	template <BaseAssetConcept _Ty>
	AssetObserver<_Ty> TryImport(const std::filesystem::path& filepath, const std::any& param = std::any());

	//* extension option *//

	template <BaseAssetConcept _Ty>
	void RegisterExtension(const std::filesystem::path& extension);

	//* getter *//

	const Storage& GetStorage() const { return storage_; }

	//* singleton *//

	static AssetStorage* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Storage storage_;

	Extensions extensions_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	template <BaseAssetConcept _Ty>
	static std::shared_ptr<_Ty> Cast(const std::shared_ptr<BaseAsset>& asset);

};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetStorage class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <BaseAssetConcept _Ty>
AssetObserver<_Ty> AssetStorage::Import(const std::filesystem::path& filepath, const std::any& param) {
	constexpr const std::type_info* type = &typeid(_Ty);

	// assetの作成
	std::shared_ptr<BaseAsset> asset = std::make_shared<_Ty>();
	asset->SetFilepath(filepath);
	asset->SetParam(param);

	storage_[type][filepath] = asset;
	SxavengerSystem::PushTask(asset->GetAsyncExecution(), asset);

	AssetObserver<_Ty> observer;
	observer.Register(AssetStorage::Cast<_Ty>(asset));

	return observer;
}

template <BaseAssetConcept _Ty>
AssetObserver<_Ty> AssetStorage::TryImport(const std::filesystem::path& filepath, const std::any& param) {
	constexpr const std::type_info* type = &typeid(_Ty);

	if (storage_[type].Contains(filepath)) {
		AssetObserver<_Ty> observer;
		observer.Register(AssetStorage::Cast<_Ty>(storage_[type].At(filepath)));

		return observer;
	}

	return AssetStorage::Import<_Ty>(filepath, param);
}

template <BaseAssetConcept _Ty>
inline void AssetStorage::RegisterExtension(const std::filesystem::path& extension) {
	constexpr const std::type_info* type = &typeid(_Ty);
	extensions_.emplace(extension, std::make_pair(type, []() { return std::make_shared<_Ty>(); }));
}

template <BaseAssetConcept _Ty>
inline std::shared_ptr<_Ty> AssetStorage::Cast(const std::shared_ptr<BaseAsset>& asset) {
	return std::static_pointer_cast<_Ty>(asset);
}

////////////////////////////////////////////////////////////////////////////////////////////
// AssetStorage class singleton
////////////////////////////////////////////////////////////////////////////////////////////
static AssetStorage* sAssetStorage = AssetStorage::GetInstance();
