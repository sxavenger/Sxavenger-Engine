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

	pathtracing_ = std::make_unique<FRenderCorePathtracing>();
	pathtracing_->Init();

	layer_ = std::make_unique<FRenderCoreLayer>();
	layer_->Init();

	process_ = std::make_unique<FRenderCoreProcess>();
	process_->Init();

	transition_ = std::make_unique<FRenderCoreTransition>();
	transition_->Init();

	brdfLut_ = SxavengerAsset::TryImport<AssetTexture>(kPackagesDirectory / "textures/rendering/brdf_lut.png", Texture::Option{ Texture::Encoding::Intensity, false });
}

void FRenderCore::Term() {
	geometry_.reset();
	light_.reset();
	pathtracing_.reset();
	layer_.reset();
	process_.reset();
	transition_.reset();
}

const D3D12_GPU_DESCRIPTOR_HANDLE& FRenderCore::GetBRDFLut() const {
	return brdfLut_.WaitGet()->GetGPUHandleSRV();
}

FRenderCore* FRenderCore::GetInstance() {
	static FRenderCore instance;
	return &instance;
}
