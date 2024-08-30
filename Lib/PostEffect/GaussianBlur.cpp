#include "GaussianBlur.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Sxavenger.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// GaussianBlur class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GaussianBlur::Init(DXGI_FORMAT format) {

	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"Sprite.VS.hlsl", GRAPHICS_VERTEX);
	blob_->Create(L"GaussianBlur/GaussianBlur.PS.hlsl", GRAPHICS_PIXEL);

	pipeline_ = std::make_unique<GraphicsPipeline>();

	GraphicsRootSignatureDesc rootDesc;
	rootDesc.SetCBV(0, VISIBILITY_PIXEL, 0);
	rootDesc.SetSRV(1, VISIBILITY_PIXEL, 0);
	rootDesc.SetSampler(0, MODE_CLAMP, VISIBILITY_PIXEL, 0);

	GraphicsPipelineDesc pipelineDesc;
	pipelineDesc.CreateDefaultDesc();

	pipelineDesc.rtvFormats.clear();
	pipelineDesc.SetRTVFormat(format);

	pipeline_->CreateRootSignature(Sxavenger::GetDevicesObj(), rootDesc);
	pipeline_->CreatePipeline(Sxavenger::GetDevicesObj(), blob_.get(), pipelineDesc);

	param_ = std::make_unique<BufferResource<BlurParameter>>(Sxavenger::GetDevicesObj(), 1);

	sprite_ = DrawMethods::Plane2D();

}

void GaussianBlur::Term() {
}

void GaussianBlur::CreateBlurTexture(
	Texture* texture, Vector2ui textureSize, Vector2ui strength, float sigma) {

	param_->operator[](0).textureSize = textureSize;
	param_->operator[](0).strength = strength;
	param_->operator[](0).sigma = sigma;

	auto commandList = Sxavenger::GetCommandList();

	pipeline_->SetPipeline(commandList);

	sprite_.SetBuffer(commandList);

	commandList->SetGraphicsRootConstantBufferView(0, param_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(1, texture->GetGPUHandleSRV());

	sprite_.DrawCall(commandList);
}
