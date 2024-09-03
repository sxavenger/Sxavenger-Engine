#include "Particle.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Sxavenger.h>
#include <SxavengerGraphics.h>
#include <Performance.h>



//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// Particle class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Particle::Init() {

	//* CS *//
	// initialize用のpipelineの生成
	csInitBlob_ = std::make_unique<CSBlob>();
	csInitBlob_->Create(L"particle/ParticleInit.CS.hlsl");

	{
		CSRootSignatureDesc desc;
		desc.SetCBV(0, 0);
		desc.SetVirtualUAV(1, 0);
		desc.SetVirtualUAV(2, 1);

		csInitPipeline_ = std::make_unique<CSPipeline>();
		csInitPipeline_->CreatePipeline(desc, csInitBlob_.get());
	}

	// Emit用のpipelineの生成
	csBlob_ = std::make_unique<CSBlob>();
	csBlob_->Create(L"particle/ParticleEmit.CS.hlsl");

	{
		CSRootSignatureDesc desc;
		desc.SetCBV(0, 0);
		desc.SetCBV(1, 1);
		desc.SetVirtualUAV(2, 0);
		desc.SetCBV(3, 2);
		desc.SetVirtualUAV(4, 1);

		csPipeline_ = std::make_unique<CSPipeline>();
		csPipeline_->CreatePipeline(desc, csBlob_.get());
	}

	// Update用のpipelineの生成
	csUpdateBlob_ = std::make_unique<CSBlob>();
	csUpdateBlob_->Create(L"particle/ParticleUpdate.CS.hlsl");
	
	{
		CSRootSignatureDesc desc;
		desc.SetCBV(0, 0);
		desc.SetCBV(1, 1);
		desc.SetVirtualUAV(2, 0);

		csUpdatePipeline_ = std::make_unique<CSPipeline>();
		csUpdatePipeline_->CreatePipeline(desc, csUpdateBlob_.get());
	}

	//* buffers *//
	particleBuffer_ = std::make_unique<UnorderedBufferResource<ParticleCS>>(Sxavenger::GetDevicesObj(), kParticleNum);

	informationBuffer_ = std::make_unique<BufferResource<uint32_t>>(Sxavenger::GetDevicesObj(), 1);
	(*informationBuffer_)[0] = kParticleNum;

	counterBuffer_ = std::make_unique<UnorderedBufferResource<int32_t>>(Sxavenger::GetDevicesObj(), 1);

	emitterBuffer_ = std::make_unique<BufferResource<EmitterSphere>>(Sxavenger::GetDevicesObj(), 1);
	(*emitterBuffer_)[0].count         = 10;
	(*emitterBuffer_)[0].frequency     = 1.0f;
	(*emitterBuffer_)[0].frequencyTime = 0.0f;
	(*emitterBuffer_)[0].position      = { 0.0f, 0.0f, 0.0f };
	(*emitterBuffer_)[0].radius        = 1.0f;
	(*emitterBuffer_)[0].isEmit        = false;

	// initCSの実行
	{
		auto commandList = Sxavenger::GetCommandList();

		csInitPipeline_->SetPipeline();

		commandList->SetComputeRootConstantBufferView(0, informationBuffer_->GetGPUVirtualAddress());
		commandList->SetComputeRootUnorderedAccessView(1, particleBuffer_->GetGPUVirtualAddress());
		commandList->SetComputeRootUnorderedAccessView(2, counterBuffer_->GetGPUVirtualAddress());

		csInitPipeline_->Dispatch(RoundUp(kParticleNum, 1024), 1, 1);
	}

	//* Graphics *//
	render_.Init();
	render_.CreateBlob(L"particle/particle.VS.hlsl", GRAPHICS_VERTEX);
	render_.CreateBlob(L"particle/particle.PS.hlsl", GRAPHICS_PIXEL);

	render_.CreateTable();

	render_.BindBuffer("gCamera", SxavengerGraphics::camera3D->GetGPUVirtualAddress());
	render_.BindBuffer("gParticle", particleBuffer_->GetGPUVirtualAddress());

	render_.CreatePipeline(kBlendModeAdd);

	//* IA *//
	plane_ = DrawMethods::Plane({1.0f, 1.0f});

	SetThisAttribute("particle");
}

void Particle::Term() {
}

void Particle::Update() {

	// emitterの更新
	auto& emitter = (*emitterBuffer_)[0]; //!< 参照の取得

	emitter.frequencyTime += Performance::GetDeltaTime(s); //!< 秒を取得

	if (emitter.frequencyTime >= emitter.frequency) { //!< 経過時間が上回ってたら
		emitter.frequencyTime -= emitter.frequency;
		emitter.isEmit = true;

	} else {
		emitter.isEmit = false;
	}

	// csの更新処理
	auto commandList = Sxavenger::GetCommandList();

	csPipeline_->SetPipeline();

	commandList->SetComputeRootConstantBufferView(0, emitterBuffer_->GetGPUVirtualAddress());
	commandList->SetComputeRootConstantBufferView(1, Performance::GetGPUVirtualAddress());
	commandList->SetComputeRootUnorderedAccessView(2, particleBuffer_->GetGPUVirtualAddress());
	commandList->SetComputeRootConstantBufferView(3, informationBuffer_->GetGPUVirtualAddress());
	commandList->SetComputeRootUnorderedAccessView(4, counterBuffer_->GetGPUVirtualAddress());

	csPipeline_->Dispatch(1, 1, 1);

	// barrier
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = particleBuffer_->GetResource();

	commandList->ResourceBarrier(1, &barrier);


	csUpdatePipeline_->SetPipeline();

	commandList->SetComputeRootConstantBufferView(0, informationBuffer_->GetGPUVirtualAddress());
	commandList->SetComputeRootConstantBufferView(1, Performance::GetGPUVirtualAddress());
	commandList->SetComputeRootUnorderedAccessView(2, particleBuffer_->GetGPUVirtualAddress());

	csUpdatePipeline_->Dispatch(1, 1, 1);

}

void Particle::Draw() {

	auto commandList = Sxavenger::GetCommandList();

	render_.SetPipeline(commandList);
	plane_.SetBuffer(commandList);

	render_.BindBuffer("gCamera", SxavengerGraphics::camera3D->GetGPUVirtualAddress()); //!< camera3Dは呼び出されるたび変わるので
	render_.BindGraphicsParameter(commandList);

	plane_.DrawCall(commandList, kParticleNum);
}

void Particle::SetAttributeImGui() {

	ImGui::Text("frequencyTime: %f", (*emitterBuffer_)[0].frequencyTime);

}
