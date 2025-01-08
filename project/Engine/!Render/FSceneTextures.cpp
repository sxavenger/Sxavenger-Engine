#include "FSceneTextures.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FSceneTextures::kGBufferLayoutCount> FSceneTextures::kGBufferFormats = {
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< Normal
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< Material
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< Albedo_AO
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< Position
};

////////////////////////////////////////////////////////////////////////////////////////////
// Framework SceneTextures class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSceneTextures::Create(const Vector2ui& size) {
	for (uint8_t i = 0; i < kGBufferLayoutCount; ++i) {
		gBuffers_[i] = std::make_unique<FTexture>();
		gBuffers_[i]->Create(size, kGBufferFormats[i]);
	}

	depth_ = std::make_unique<FSceneDepth>();
	depth_->Create(size);

	parameterBuffer_ = std::make_unique<DxObject::DimensionBuffer<TexturesParameter>>();
	parameterBuffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameterBuffer_->At(0).size = size;

}
