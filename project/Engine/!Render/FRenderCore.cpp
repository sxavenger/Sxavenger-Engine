#include "FRenderCore.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerDirectory.h>
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCore class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCore::Init() {
	geometry_ = std::make_unique<FRenderCoreGeometry>();
	geometry_->Init();

	light_ = std::make_unique<FRenderCoreLight>();
	light_->Init();
}

void FRenderCore::Term() {
	geometry_.reset();
	light_.reset();
}

FRenderCore* FRenderCore::GetInstance() {
	static FRenderCore instance;
	return &instance;
}
