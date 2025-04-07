#include "FRenderCore.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerConfig.h>
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCore class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCore::Init() {
	geometry_ = std::make_unique<FRenderCoreGeometry>();
	geometry_->Init();

	light_ = std::make_unique<FRenderCoreLight>();
	light_->Init();

	raytracing_ = std::make_unique<FRenderCoreRaytracing>();
	raytracing_->Init();

	layer_ = std::make_unique<FRenderCoreLayer>();
	layer_->Init();

	process_ = std::make_unique<FRenderCoreProcess>();
	process_->Init();

	brdfLut_ = SxavengerAsset::TryImport<AssetTexture>(kPackagesDirectory / "textures/rendering/brdf_lut.png");
}

void FRenderCore::Term() {
	geometry_.reset();
	light_.reset();
	raytracing_.reset();
	layer_.reset();
	process_.reset();
}

const D3D12_GPU_DESCRIPTOR_HANDLE& FRenderCore::GetBRDFLut() const {
	return brdfLut_.WaitGet()->GetGPUHandleSRV();
}

FRenderCore* FRenderCore::GetInstance() {
	static FRenderCore instance;
	return &instance;
}
