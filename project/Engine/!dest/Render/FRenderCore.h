#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* core
#include "Core/FRenderCoreGeometry.h"
#include "Core/FRenderCoreLight.h"
#include "Core/FRenderCoreRaytracing.h"
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

	FRenderCoreRaytracing* GetRaytracing() { return raytracing_.get(); }

	FRenderCoreLayer* GetLayer() { return layer_.get(); }

	FRenderCoreProcess* GetProcess() { return process_.get(); }

	//* singleton *//

	static FRenderCore* GetInstance();
	
private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<FRenderCoreGeometry>   geometry_;
	std::unique_ptr<FRenderCoreLight>      light_;
	std::unique_ptr<FRenderCoreRaytracing> raytracing_;
	std::unique_ptr<FRenderCoreLayer>      layer_;
	std::unique_ptr<FRenderCoreProcess>    process_;

};
