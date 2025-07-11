#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* core
#include "Core/FRenderCoreGeometry.h"
#include "Core/FRenderCoreLight.h"
#include "Core/FRenderCorePathtracing.h"
#include "Core/FRenderCoreLayer.h"
#include "Core/FRenderCoreProcess.h"

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

	FRenderCorePathtracing* GetPathtracing() { return pathtracing_.get(); }

	FRenderCoreLayer* GetLayer() { return layer_.get(); }

	FRenderCoreProcess* GetProcess() { return process_.get(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetBRDFLut() const;

	//* singleton *//

	static FRenderCore* GetInstance();
	
private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<FRenderCoreGeometry>    geometry_;
	std::unique_ptr<FRenderCoreLight>       light_;
	std::unique_ptr<FRenderCorePathtracing> pathtracing_;
	std::unique_ptr<FRenderCoreLayer>       layer_;
	std::unique_ptr<FRenderCoreProcess>     process_;

	//* textures *//

	AssetObserver<AssetTexture> brdfLut_;

};
