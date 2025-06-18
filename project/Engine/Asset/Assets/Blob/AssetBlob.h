#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../BaseAsset.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxShaderBlob.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// Asset Blob class
//////////////////////////////////////////////////////////////////////////////////////////////
class AssetBlob
	: public BaseAsset, public DxObject::ShaderBlob {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetBlob()  = default;
	~AssetBlob() = default;

	void Load(_MAYBE_UNUSED const DirectXThreadContext* context) override; //!< entry point は L"main"限定

	AsyncExecution GetAsyncExecution() const override { return AsyncExecution::None; }

	//* inspector option *//

	void ShowInspector() override;
	
private:
};
