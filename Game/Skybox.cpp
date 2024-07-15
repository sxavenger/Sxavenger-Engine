#include "Skybox.h"

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// Skybox class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Skybox::Init() {

	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"", GRAPHICS_VERTEX);
	blob_->Create(L"", GRAPHICS_PIXEL);

	pipeline_ = std::make_unique<GraphicsPipeline>();


	skybox_ = DrawMethods::SkyBox({2.0f, 2.0f, 2.0f});

}

void Skybox::Term() {
}
