#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../BaseAsset.h"
#include "Texture.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// AssetTexture class
//////////////////////////////////////////////////////////////////////////////////////////////
class AssetTexture
	: public BaseAsset, public Texture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetTexture()           = default;
	~AssetTexture() override = default;

	void Load(const DirectXThreadContext* context) override;

	void ShowInspector() override;

private:
};
