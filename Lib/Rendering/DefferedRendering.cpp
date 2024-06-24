#include "DefferedRendering.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>
#include <Environment.h>

//=========================================================================================
// static variables
//=========================================================================================

const DXGI_FORMAT DefferedRendering::formats_[DefferedRenderingType::kCountOfDefferedRenderingType] = {
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< DEFFERERD_ALBED
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< DEFFERERD_DEPTH
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< DEFFERERD_NORMAL
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< DEFFERERD_WORLD
};

////////////////////////////////////////////////////////////////////////////////////////////
// DefferedRendering class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DefferedRendering::Init() {

	for (int i = 0; i < DefferedRenderingType::kCountOfDefferedRenderingType; ++i) {
		// deffered用 renderTextureの生成
		defferedTextures_[i] = std::make_unique<RenderTexture>();
		defferedTextures_[i]->Create(
			MyEngine::GetDxCommon(),
			kWindowWidth, kWindowHeight, { 0.0f }, formats_[i]
		);

		// 外部参照用のptr配列の設定
		texturePtrs_[i] = defferedTextures_[i].get();
	}


}

void DefferedRendering::Term() {
}
