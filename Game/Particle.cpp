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

void Particle::Init(DefferedRendering* defferdRendering) {
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
		graphicsBlob_->Create(L"particleDeffered.PS.hlsl", GRAPHICS_PIXEL);

		GraphicRootSignatureDesc desc;
		desc.Resize(2, 0);
		desc.SetCBV(0, VISIBILITY_VERTEX, 0);
		desc.SetVirtualSRV(1, VISIBILITY_VERTEX, 0);

		graphicsPipeline_ = std::make_unique<GraphicsPipeline>();
		graphicsPipeline_->CreateRootSiganture(MyEngine::GetDevicesObj(), desc);
		graphicsPipeline_->CreatePipeline(MyEngine::GetDevicesObj(), graphicsBlob_.get(), kBlendModeNormal, kCountOfDefferedRenderingType, defferdRendering->GetFormats());

	}

	// IA
	model_ = std::make_unique<Model>("./Resources/model", "sphere.obj");

	SetThisAttribute("particle");
}

void Particle::Term() {
}

void Particle::Update() {

	auto commandList = MyEngine::GetCommandList();

	// update cs
	csPipelineState_->SetCSPipeline();

	commandList->SetComputeRootUnorderedAccessView(0, csBuffer_->GetGPUVirtualAddress());

	csPipelineState_->Dispatch(2, 2, 2);

	/*isUpdate = false;*/
}

void Particle::Draw() {

	auto commandList = MyEngine::GetCommandList();

	graphicsPipeline_->SetPipeline(commandList);

	model_->SetBuffers(commandList, 0);

	commandList->SetGraphicsRootConstantBufferView(0, MyEngine::camera3D_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(1, csBuffer_->GetGPUVirtualAddress());

	model_->DrawCall(commandList, 0, csBuffer_->GetIndexSize());

}

void Particle::SetAttributeImGui() {
}
