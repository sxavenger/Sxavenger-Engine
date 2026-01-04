#include "FRenderCore.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/Assets/Content/ContentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCore class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCore::Init() {
	geometry_ = std::make_unique<FRenderCoreGeometry>();
	geometry_->Init();

	light_ = std::make_unique<FRenderCoreLight>();
	light_->Init();

	restir_ = std::make_unique<FRenderCoreRestir>();
	restir_->Init();

	probe_ = std::make_unique<FRenderCoreProbe>();
	probe_->Init();

	canvas_ = std::make_unique<FRenderCoreCanvas>();
	canvas_->Init();

	process_ = std::make_unique<FRenderCoreProcess>();
	process_->Init();

	transition_ = std::make_unique<FRenderCoreTransition>();
	transition_->Init();

	brdfLut_ = sContentStorage->Import<ContentTexture>(kPackagesDirectory / "textures/rendering/brdf_lut.png",   ContentTexture::Option{ ContentTexture::Encoding::Intensity, false })->GetId();

	smaaArea_   = sContentStorage->Import<ContentTexture>(kPackagesDirectory / "textures/rendering/smaaAreaTex.dds", ContentTexture::Option{ ContentTexture::Encoding::Intensity, false })->GetId();
	smaaSearch_ = sContentStorage->Import<ContentTexture>(kPackagesDirectory / "textures/rendering/smaaSearchTex.dds", ContentTexture::Option{ ContentTexture::Encoding::Intensity, false })->GetId();
}

void FRenderCore::Term() {
	geometry_.reset();
	light_.reset();
	restir_.reset();
	probe_.reset();
	canvas_.reset();
	process_.reset();
	transition_.reset();
}

FRenderCore* FRenderCore::GetInstance() {
	static FRenderCore instance;
	return &instance;
}
