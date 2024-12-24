#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../BaseAsset.h"

//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Content/Texture/Texture.h>

//* c++
#include <unordered_set>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetTexture
	: public BaseAsset, public Texture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Load(const DirectXThreadContext* context);
	//!< single thread only.

	void SetInspectorImGui() override;

	static bool CheckExtension(const std::filesystem::path& extension);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================
	
	//* extension *//

	static const std::unordered_set<std::filesystem::path> extension_;
};
