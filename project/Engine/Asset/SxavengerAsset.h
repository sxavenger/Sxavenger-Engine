#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "Asset.h"

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

	static Asset::Files Import(const std::filesystem::path& filepath);

	static std::shared_ptr<AssetTexture> ImportTexture(const std::filesystem::path& filepath);

	static std::shared_ptr<AssetModel> ImportModel(const std::filesystem::path& filepath);

	static Asset* GetAsset();

private:
};