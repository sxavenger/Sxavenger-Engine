#include "Bloom.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>
#include <Environment.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Bloom class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Bloom::Init() {

	mainRenderingTexture_= std::make_unique<RenderTexture>();
	mainRenderingTexture_->Create(MyEngine::GetDxCommon(), kWindowWidth, kWindowHeight, defaultClearColor, DXGI_FORMAT_R32G32B32A32_FLOAT);

	luminanceTexture_ = std::make_unique<RenderTexture>();
	luminanceTexture_->Create(MyEngine::GetDxCommon(), kWindowWidth, kWindowHeight, defaultClearColor, DXGI_FORMAT_R32G32B32A32_FLOAT);

}

void Bloom::Term() {
}
