#include "FSceneTextures.h"

const std::array<DXGI_FORMAT, FSceneTextures::kGBufferLayoutCount> kGBufferFormats = {
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
		gBuffers_[i] = std::make_unique<MultiViewTextureBuffer>();
		gBuffers_[i]->Create(VIEWFLAG_UAV | VIEWFLAG_RTV, size, {}, kGBufferFormats[i]);
	}

	depth_ = std::make_unique<FSceneDepth>();
	depth_->Create(size);
}
