#include "Particle.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>
#include "DxShaderReflection.h"
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
	csInitBlob_->Init(L"particle/ParticleInit.CS.hlsl");

	{
		CSRootSignatureDesc desc;
		desc.Resize(2, 0);
		desc.SetCBV(0, 0);
		desc.SetVirtualUAV(1, 0);

		csInitPipeline_ = std::make_unique<CSPipelineState>();
		csInitPipeline_->Init(desc, csInitBlob_.get());
	}

	// Update用のpipelineの生成
	csBlob_ = std::make_unique<CSBlob>();
	csBlob_->Init(L"particle/Particle.CS.hlsl");

	{
		CSRootSignatureDesc desc;
		desc.Resize(3, 0);
		desc.SetCBV(0, 0);
		desc.SetCBV(1, 1);
		desc.SetVirtualUAV(2, 0);

		csPipeline_ = std::make_unique<CSPipelineState>();
		csPipeline_->Init(desc, csBlob_.get());
	}

	//* buffers *//
	particleBuffer_ = std::make_unique<CSBufferResource<ParticleCS>>(MyEngine::GetDevicesObj(), kParticleNum);

	informationBuffer_ = std::make_unique<BufferResource<uint32_t>>(MyEngine::GetDevicesObj(), 1);
	(*informationBuffer_)[0] = kParticleNum;

	emitterBuffer_ = std::make_unique<BufferResource<EmitterSphere>>(MyEngine::GetDevicesObj(), 1);
	(*emitterBuffer_)[0].count         = 10;
	(*emitterBuffer_)[0].frequency     = 1.0f;
	(*emitterBuffer_)[0].frequencyTime = 0.0f;
	(*emitterBuffer_)[0].position      = { 0.0f, 0.0f, 0.0f };
	(*emitterBuffer_)[0].radius        = 1.0f;
	(*emitterBuffer_)[0].isEmit        = false;

	// initCSの実行
	{
		auto commandList = MyEngine::GetCommandList();

		csInitPipeline_->SetCSPipeline();

		commandList->SetComputeRootConstantBufferView(0, informationBuffer_->GetGPUVirtualAddress());
		commandList->SetComputeRootUnorderedAccessView(1, particleBuffer_->GetGPUVirtualAddress());

		csInitPipeline_->Dispatch(RoundUp(kParticleNum, 1024), 1, 1);
	}

	//* Graphics *//
	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"particle/particle.VS.hlsl", GRAPHICS_VERTEX);
	blob_->Create(L"particle/particle.PS.hlsl", GRAPHICS_PIXEL);

	{
		pipeline_ = std::make_unique<GraphicsPipeline>();

		GraphicsRootSignatureDesc rootDesc;
		rootDesc.Resize(2, 0);
		rootDesc.SetCBV(0, VISIBILITY_VERTEX, 0);
		rootDesc.SetVirtualSRV(1, VISIBILITY_VERTEX, 0);

		pipeline_->CreateRootSignature(MyEngine::GetDevicesObj(), rootDesc);

		GraphicsPipelineDesc pipelineDesc;
		pipelineDesc.CreateDefaultDesc();
		pipelineDesc.blendMode = kBlendModeAdd;

		pipeline_->CreatePipeline(MyEngine::GetDevicesObj(), blob_.get(), pipelineDesc);
	}

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
	auto commandList = MyEngine::GetCommandList();

	csPipeline_->SetCSPipeline();

	commandList->SetComputeRootConstantBufferView(0, emitterBuffer_->GetGPUVirtualAddress());
	commandList->SetComputeRootConstantBufferView(1, Performance::GetGPUVirtualAddress());
	commandList->SetComputeRootUnorderedAccessView(2, particleBuffer_->GetGPUVirtualAddress());

	csPipeline_->Dispatch(1, 1, 1);

}

void Particle::Draw() {

	auto commandList = MyEngine::GetCommandList();

	pipeline_->SetPipeline(commandList);
	plane_.SetBuffer(commandList);

	commandList->SetGraphicsRootConstantBufferView(0, MyEngine::camera3D_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(1, particleBuffer_->GetGPUVirtualAddress());

	plane_.DrawCall(commandList, (*emitterBuffer_)[0].count);
}

void Particle::SetAttributeImGui() {

	ImGui::Text("frequencyTime: %f", (*emitterBuffer_)[0].frequencyTime);

}
