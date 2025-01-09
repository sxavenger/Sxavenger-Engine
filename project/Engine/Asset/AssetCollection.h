#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "Unknown/AssetUnknown.h"
#include "Texture/AssetTexture.h"
#include "Model/AssetModel.h"
#include "Animator/AssetAnimator.h"
#include "Blob/AssetBlob.h"

//* asset thread
#include "Thread/AsyncAssetThread.h"

//* lib
#include <Lib/Sxl/OptimizedLowerPathMap.h>

//* c++
#include <filesystem>
#include <memory>
#include <variant>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetCollection
	: public AsyncAssetThreadCollection {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// FileType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class FileType {
		Unknown,
		Texture,
		Model_Animator,
		Shader,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// File variant
	////////////////////////////////////////////////////////////////////////////////////////////
	using File = std::variant <
		std::shared_ptr<AssetUnknown>,
		std::shared_ptr<AssetTexture>,
		std::pair<std::shared_ptr<AssetModel>, std::shared_ptr<AssetAnimator>>,
		std::shared_ptr<AssetBlob>
	>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// FileData using
	////////////////////////////////////////////////////////////////////////////////////////////
	using FileData = std::pair<FileType, File>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// FolderData using
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Folder;
	using FolderData = std::pair<std::filesystem::path, Folder>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Folder structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Folder {

		//* member *//

		Sxl::OptimizedLowerPathMap<FileData>   files;
		Sxl::OptimizedLowerPathMap<FolderData> folders;

		//* methods *//

		bool IsEmpty() const { return files.Empty() && folders.Empty(); };

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetCollection()  = default;
	~AssetCollection() { Term(); }

	void Init();

	void Term();

	//* import *//

	template <BaseAssetConcept T>
	void Import(const std::filesystem::path& filepath);

	//* search *//

	const FileData& GetFileData(const std::filesystem::path& filepath) const;
	FileData& GetFileData(const std::filesystem::path& filepath);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* assets *//

	FolderData root_ = {};

	//* next load option *//

	std::optional<DxObject::CompileProfile> profile_ = std::nullopt;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static FileType GetFileType(const std::filesystem::path& filepath);

	FileData CreateFileData(const std::filesystem::path& filepath);

	template <BaseAssetConcept T>
	std::shared_ptr<T> ConvertAsset(const std::shared_ptr<BaseAsset>& base);

};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCollection class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <BaseAssetConcept T>
void Import(const std::filesystem::path& filepath) {
	
}
