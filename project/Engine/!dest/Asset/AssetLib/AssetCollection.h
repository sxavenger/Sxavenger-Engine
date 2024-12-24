#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset files
#include "Texture/AssetTexture.h"
#include "ModelAnimation/AssetModelAnimation.h"
#include "Unknown/AssetUnknown.h"

//* lib
#include <Lib/Sxl/LowerPathMap.h>

//* c++
#include <filesystem>
#include <variant>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetFile Type
////////////////////////////////////////////////////////////////////////////////////////////

using AssetFile = std::variant<
	AssetTexture,
	AssetModelAnimation,
	AssetUnknown
>;

template <class T>
concept IsAssetFile = std::is_same_v<T, AssetFile>;

////////////////////////////////////////////////////////////////////////////////////////////
// AssetFolder structure
////////////////////////////////////////////////////////////////////////////////////////////
struct AssetFolder {
	Sxl::OptimizerdMap<std::filesystem::path, AssetFolder>                folders;
	Sxl::OptimizerdMap<std::filesystem::path, std::shared_ptr<AssetFile>> files;
};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetCollection {
public:

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetCollection()  = default;
	~AssetCollection() = default;

	//* import methods *//

	std::shared_ptr<AssetFile> Import(const std::filesystem::path& filepath);

	std::shared_ptr<AssetTexture> ImportTexture(const std::filesystem::path& filepath);

	std::shared_ptr<AssetModelAnimation> ImportModelAnimation(const std::filesystem::path& filepath);
	std::shared_ptr<AssetModel> ImportModel(const std::filesystem::path& filepath);
	std::shared_ptr<AssetAnimation> ImportAnimation(const std::filesystem::path& filepath);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	AssetFolder root_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	template <class T> requires std::is_base_of_v<BaseAsset, T>
	static std::shared_ptr<AssetFile> CreateAssetFile(const std::filesystem::path& filepath);

	template <class T>
	static std::shared_ptr<T> ConvertTo(const std::shared_ptr<AssetFile>& file);

	std::shared_ptr<AssetFile> ImportFile(const std::filesystem::path& filepath);

};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCollection class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class T > requires std::is_base_of_v<BaseAsset, T>
std::shared_ptr<AssetFile> AssetCollection::CreateAssetFile(const std::filesystem::path& filepath) {
	auto asset = std::make_shared<T>();
	asset->SetFilepath(filepath);
	return std::make_shared<AssetFile>(std::in_place_type<T>, *asset);
}

template <class T>
inline std::shared_ptr<T> AssetCollection::ConvertTo(const std::shared_ptr<AssetFile>& file) {
	T* asset = std::get_if<T>(file.get());
	Assert(asset != nullptr, "asset type is not collect.");

	return std::make_shared<T>(asset, file);
}
