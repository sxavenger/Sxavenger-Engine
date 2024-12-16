#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../IAsset.h"

//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Content/Texture/Texture.h>

//* c++
#include <filesystem>
#include <unordered_set>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetTexture
	: public IAsset, public Texture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Load(const DirectXThreadContext* context);

	void SetFilepath(const std::filesystem::path& filepath) { filepath_ = filepath; }

	void SetInspectorImGui() override;

	static bool CheckExtension(const std::filesystem::path& extension);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::filesystem::path filepath_;

	bool isLoad_ = false;

	//* extension *//

	static const std::unordered_set<std::filesystem::path> extension_;

};