#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "../BaseAsset.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxShaderBlob.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetBlob class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetBlob
	: public BaseAsset {
public:


	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetBlob()  = default;
	~AssetBlob() = default;

	void Load(_MAYBE_UNUSED const DirectXThreadContext* context) override; //!< entry point は L"main"限定

	void SetProfile(DxObject::CompileProfile profile) { profile_ = profile; }

	//* getter *//

	const DxObject::ShaderBlob& GetBlob() const { return blob_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* blob *//

	DxObject::ShaderBlob blob_;

	//* input parameter *//

	DxObject::CompileProfile profile_;

};
