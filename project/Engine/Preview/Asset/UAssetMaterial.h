#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "UBaseAsset.h"
#include "UAssetTexture.h"

//* engine
#include <Engine/Content/Material/Material.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetMaterial class
////////////////////////////////////////////////////////////////////////////////////////////
class UAssetMaterial final
	: public UBaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UAssetMaterial()           = default;
	~UAssetMaterial() override = default;

	//* --- option *//

	//* sasset option *//

	json Serialize() const override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* asset data *//

	Material material_;

	//* parameter *//

	/* note: parameterの設定
	 - texture(Uuid)
	 - value(型はバラバラ)

	*/

	

};
