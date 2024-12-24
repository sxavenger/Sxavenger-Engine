#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "AssetLib/AssetCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerAsset class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void Term();

	static std::weak_ptr<AssetFile> Import(const std::filesystem::path& filepath);

	/*static std::shared_ptr<AssetTexture> ImportTexture(const std::filesystem::path& filepath);

	static std::shared_ptr<AssetModel> ImportModel(const std::filesystem::path& filepath);*/

	static AssetCollection* GetAsset();

private:
};
