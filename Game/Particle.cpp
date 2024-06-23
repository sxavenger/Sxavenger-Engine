#include "Particle.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// Particle class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Particle::Init() {

	// cs
	{
		csBlob_ = std::make_unique<CSBlob>();
		csBlob_->Init(L"particle.CS.hlsl");

		CSRootSignatureDesc desc;
		desc.Resize(1, 0);
		desc.SetVirtualUAV(0, 0);

		csPipelineState_ = std::make_unique<CSPipelineState>();
		csPipelineState_->Init(desc, csBlob_.get());

		csBuffer_ = std::make_unique<CSBufferResource<ParticleData>>(MyEngine::GetDevicesObj(), 16 * 16 * 16);
	}

	// graphics
	{
		graphicsBlob_ = std::make_unique<GraphicsBlob>();
		graphicsBlob_->Create(L"particle.VS.hlsl", GRAPHICS_VERTEX);
		graphicsBlob_->Create(L"particle.PS.hlsl", GRAPHICS_PIXEL);

		GraphicRootSignatureDesc desc;
		desc.Resize(2, 0);
		desc.SetCBV(0, VISIBILITY_VERTEX, 0);
		desc.SetVirtualSRV(1, VISIBILITY_VERTEX, 0);

		graphicsPipeline_ = std::make_unique<GraphicsPipeline>();
		graphicsPipeline_->CreateRootSiganture(MyEngine::GetDevicesObj(), desc);
		graphicsPipeline_->CreatePipeline(MyEngine::GetDevicesObj(), graphicsBlob_.get(), kBlendModeNormal);

	}

	// IA
	model_ = std::make_unique<Model>("./Resources/model", "plane.obj");

	SetThisAttribute("particle");

}

void Particle::Term() {
}

void Particle::Update() {

	auto commadList = MyEngine::GetCommandList();

	// update cs
	csPipelineState_->SetCSPipeline();

	commadList->SetComputeRootUnorderedAccessView(0, csBuffer_->GetGPUVirtualAddress());

	csPipelineState_->Dispatch(2, 2, 2);

	/*isUpdate = false;*/
}

void Particle::Draw() {

	auto commadList = MyEngine::GetCommandList();

	graphicsPipeline_->SetPipeline(commadList);

	model_->SetBuffers(commadList, 0);

	commadList->SetGraphicsRootConstantBufferView(0, MyEngine::camera3D_->GetGPUVirtualAddress());
	commadList->SetGraphicsRootShaderResourceView(1, csBuffer_->GetGPUVirtualAddress());

	model_->DrawCall(commadList, 0, csBuffer_->GetIndexSize());

}

void Particle::SetAttributeImGui() {
}
