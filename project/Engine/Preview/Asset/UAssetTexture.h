#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "UBaseAsset.h"

//* engine
#include <Engine/Asset/Assets/Texture/Texture.h> //!< 移動予定

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class UAssetTexture final
	: public UBaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UAssetTexture()  = default;
	~UAssetTexture() { texture_.Term(); }

	//* --- option *//

	void Load(const DirectXThreadContext* context) override;

	//* sasset option *//

	json Serialize() const override;

	void Deserialize(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* asset data *//

	Texture texture_;

	//* parameter *//

	std::filesystem::path filepath_;

};
