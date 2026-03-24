#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderCore.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/Assets/Asset/AssetTexture.h>
#include <Engine/Assets/Asset/AssetParameter.h>

//* lib
#include <Lib/Sxl/SubclassRegistry.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCore class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCore final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	template <typename T> requires std::derived_from<T, FBaseRenderCore>
	T* EnsureRenderCore();

	//* singleton *//

	static FRenderCore* GetInstance();

	//* getter *//

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetBRDFLut() const { return brdfLut_.WaitGet()->GetGPUHandleSRV(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetSMAAAreaTexture() const { return smaaArea_.WaitGet()->GetGPUHandleSRV(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetSMAASearchTexture() const { return smaaSearch_.WaitGet()->GetGPUHandleSRV(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* render core *//

	Sxl::SubclassRegistry<FBaseRenderCore> registry_;

	//* texture *//

	AssetParameter<AssetTexture> brdfLut_;
	AssetParameter<AssetTexture> smaaArea_;
	AssetParameter<AssetTexture> smaaSearch_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCore class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T> requires std::derived_from<T, FBaseRenderCore>
inline T* FRenderCore::EnsureRenderCore() {

	if (!registry_.Contains<T>()) {
		//!< 存在しない場合は生成して初期化する.
		registry_.Insert<T>(); 
		registry_.Get<T>()->Init();
	}

	return registry_.Get<T>();
}

SXAVENGER_ENGINE_NAMESPACE_END


