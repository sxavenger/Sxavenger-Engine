#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* core
#include "Core/FRenderCoreGeometry.h"
#include "Core/FRenderCoreLight.h"
#include "Core/FRenderCoreRestir.h"
#include "Core/FRenderCoreCanvas.h"
#include "Core/FRenderCoreProcess.h"
#include "Core/FRenderCoreTransition.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/Assets/Asset/AssetTexture.h>
#include <Engine/Assets/Asset/AssetParameter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCore class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCore {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderCore()  = default;
	~FRenderCore() = default;

	void Init();

	void Term();

	//* getter *//

	FRenderCoreGeometry* GetGeometry() { return geometry_.get(); }

	FRenderCoreLight* GetLight() { return light_.get(); }

	FRenderCoreRestir* GetRestir() { return restir_.get(); }

	FRenderCoreCanvas* GetCanvas() { return canvas_.get(); }

	FRenderCoreProcess* GetProcess() { return process_.get(); }

	FRenderCoreTransition* GetTransition() { return transition_.get(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetBRDFLut() const { return brdfLut_.WaitGet()->GetGPUHandleSRV(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetSMAAAreaTexture() const { return smaaArea_.WaitGet()->GetGPUHandleSRV(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetSMAASearchTexture() const { return smaaSearch_.WaitGet()->GetGPUHandleSRV(); }

	//* singleton *//

	static FRenderCore* GetInstance();
	
private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<FRenderCoreGeometry>    geometry_;
	std::unique_ptr<FRenderCoreLight>       light_;
	std::unique_ptr<FRenderCoreRestir>      restir_;
	std::unique_ptr<FRenderCoreCanvas>      canvas_;
	std::unique_ptr<FRenderCoreProcess>     process_;
	std::unique_ptr<FRenderCoreTransition>  transition_;

	//* textures *//

	AssetParameter<AssetTexture> brdfLut_;

	AssetParameter<AssetTexture> smaaArea_;
	AssetParameter<AssetTexture> smaaSearch_;

};

SXAVENGER_ENGINE_NAMESPACE_END
