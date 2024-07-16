#include "Skybox.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// Skybox class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Skybox::Init() {

	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"skybox/skybox.VS.hlsl", GRAPHICS_VERTEX);
	blob_->Create(L"skybox/skybox.PS.hlsl", GRAPHICS_PIXEL);

	pipeline_ = std::make_unique<GraphicsPipeline>();
	GraphicsRootSignatureDesc rootDesc;
	rootDesc.Resize(4, 1);
	rootDesc.SetCBV(0, VISIBILITY_VERTEX, 0);
	rootDesc.SetCBV(1, VISIBILITY_VERTEX, 1);
	rootDesc.SetSRV(2, VISIBILITY_PIXEL, 0);
	rootDesc.SetCBV(3, VISIBILITY_PIXEL, 0);
	rootDesc.SetSampler(0, MODE_WRAP, VISIBILITY_PIXEL, 0);

	pipeline_->CreateRootSignature(MyEngine::GetDevicesObj(), rootDesc);

	GraphicsPipelineDesc pipelineDesc;
	pipelineDesc.CreateDefaultDesc();
	pipelineDesc.depthStencilDesc.DepthEnable    = true;
	pipelineDesc.depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; //!< 全pixelがz=1に出力されるので, 書き込む必要がない

	pipeline_->CreatePipeline(MyEngine::GetDevicesObj(), blob_.get(), pipelineDesc);

	skybox_ = DrawMethods::SkyBox({24.0f, 24.0f, 24.0f});

	matrixBuffer_ = std::make_unique<DxObject::BufferResource<Matrix4x4>>(MyEngine::GetDevicesObj(), 1);
	(*matrixBuffer_)[0] = Matrix4x4::Identity();

	vignetteBuffer_ = std::make_unique<DxObject::BufferPtrResource<float>>(MyEngine::GetDevicesObj(), 1);
	vignetteBuffer_->SetPtr(0, &strength_);

	MyEngine::LoadTexture("./resources/rostock_laage_airport_4k.dds");

	SetThisAttribute("skybox");

}

void Skybox::Term() {
}

void Skybox::Draw() {

	auto commandList = MyEngine::GetCommandList();

	pipeline_->SetPipeline(commandList);

	skybox_.SetBuffer(commandList);

	commandList->SetGraphicsRootConstantBufferView(0, MyEngine::camera3D_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matrixBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(2, MyEngine::GetTextureHandleGPU("./resources/rostock_laage_airport_4k.dds"));
	commandList->SetGraphicsRootConstantBufferView(3, vignetteBuffer_->GetGPUVirtualAddress());

	skybox_.DrawCall(commandList);

}

void Skybox::SetAttributeImGui() {
	ImGui::DragFloat("vignette strength", &strength_, 0.01f, 0.0f, 3.0f);
}
