#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "BaseAsset.h"
#include "Texture/Texture.h"
#include "Unknown/AssetUnknown.h"

//* engine
#include <Engine/System/Utility/Logger.h>

//* lib
#include <Lib/Sxl/OptimizedMap.h>

//* c++
#include <filesystem>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetCollection {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// FileType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class FileType {
		Unknown,
		Texture,
		Model,     //*
		Animatior, //* 同じになるかも
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// File using
	////////////////////////////////////////////////////////////////////////////////////////////
	using File = std::pair<FileType, std::shared_ptr<BaseAsset>>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Folder structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Folder {
		Sxl::OptimizerdMap<std::filesystem::path, File>   files;
		Sxl::OptimizerdMap<std::filesystem::path, Folder> folder;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetCollection()  = default;
	~AssetCollection() = default;

	//* import methods *//

	std::weak_ptr<BaseAsset> Import(const std::filesystem::path& filepath);

	std::weak_ptr<Texture> ImportTexture(const std::filesystem::path& filepath);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* assets *//

	Folder root_ = {};

	//* thread *//

	//=========================================================================================
	// private methods
	//=========================================================================================


	static FileType GetFileType(const std::filesystem::path& filepath);

	static File CreateFileData(const std::filesystem::path& filepath);

	template <class T> requires std::is_base_of_v<BaseAsset, T>
	static std::shared_ptr<T> ConvertAsset(const std::shared_ptr<BaseAsset>& base);

};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCollection class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class T> requires std::is_base_of_v<BaseAsset, T>
inline std::shared_ptr<T> AssetCollection::ConvertAsset(const std::shared_ptr<BaseAsset>& base) {

	std::shared_ptr<T> result = std::dynamic_pointer_cast<T>(base)
	Assert(result != nullptr, "base asset type is different.");

	return result;
}
