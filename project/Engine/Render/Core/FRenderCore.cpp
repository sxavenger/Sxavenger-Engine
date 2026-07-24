#include "FRenderCore.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FRenderCoreTransition.h"

//* engine
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/Assets/Content/ContentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCore class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCore::Init() {

	FRenderCore::EnsureRenderCore<FRenderCoreTransition>();

	brdfLut_    = sContentStorage->Import<ContentTexture>(kPackagesDirectory / "textures/rendering/brdf_lut.png",   ContentTexture::Option{ ContentTexture::Encoding::Intensity, false })->GetId();
	smaaArea_   = sContentStorage->Import<ContentTexture>(kPackagesDirectory / "textures/rendering/smaaAreaTex.dds", ContentTexture::Option{ ContentTexture::Encoding::Intensity, false })->GetId();
	smaaSearch_ = sContentStorage->Import<ContentTexture>(kPackagesDirectory / "textures/rendering/smaaSearchTex.dds", ContentTexture::Option{ ContentTexture::Encoding::Intensity, false })->GetId();

}

void FRenderCore::Term() {
	registry_.Clear();
}

FRenderCore* FRenderCore::GetInstance() {
	static FRenderCore instance;
	return &instance;
}
