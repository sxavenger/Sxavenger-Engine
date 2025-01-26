#include "FBaseRenderer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FBaseRenderer class methods
////////////////////////////////////////////////////////////////////////////////////////////

Sxl::Flag<FBaseRenderer::RenderStatus, uint8_t> FBaseRenderer::CheckStatus() const {

	Sxl::Flag<RenderStatus, uint8_t> result = RenderStatus::Succeed;

	if (textures_ == nullptr) {
		result |= RenderStatus::FSceneTextures_Error;
	}

	if (scene_ == nullptr) {
		result |= RenderStatus::FScene_Error;
	}

	if (camera_ == nullptr) {
		result |= RenderStatus::ACameraActor_Error;
	}

	//* vvv Warningでも続行する処理 vvv *//

	if (setting_ == nullptr) {
		result |= RenderStatus::FPostProcessSetting_Warning;
	}

	return result;
}
