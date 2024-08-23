#include "Particle.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>
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
		desc.Resize(3, 0);
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
		desc.Resize(5, 0);
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
		desc.Resize(3, 0);
		desc.SetCBV(0, 0);
		desc.SetCBV(1, 1);
		desc.SetVirtualUAV(2, 0);

		csUpdatePipeline_ = std::make_unique<CSPipeline>();
		csUpdatePipeline_->CreatePipeline(desc, csUpdateBlob_.get());
	}

	//* buffers *//
	particleBuffer_ = std::make_unique<UnorderedBufferResource<ParticleCS>>(MyEngine::GetDevicesObj(), kParticleNum);

	informationBuffer_ = std::make_unique<BufferResource<uint32_t>>(MyEngine::GetDevicesObj(), 1);
	(*informationBuffer_)[0] = kParticleNum;

	counterBuffer_ = std::make_unique<UnorderedBufferResource<int32_t>>(MyEngine::GetDevicesObj(), 1);

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

		csInitPipeline_->SetPipeline();

		commandList->SetComputeRootConstantBufferView(0, informationBuffer_->GetGPUVirtualAddress());
		commandList->SetComputeRootUnorderedAccessView(1, particleBuffer_->GetGPUVirtualAddress());
		commandList->SetComputeRootUnorderedAccessView(2, counterBuffer_->GetGPUVirtualAddress());

		csInitPipeline_->Dispatch(RoundUp(kParticleNum, 1024), 1, 1);
	}

	//* Graphics *//
	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"particle/particle.VS.hlsl", GRAPHICS_VERTEX);
	blob_->Create(L"particle/particle.PS.hlsl", GRAPHICS_PIXEL);

	reflection_ = std::make_unique<ShaderReflectionTable>();
	reflection_->Create(blob_->GetGraphicsBlobs()[GRAPHICS_VERTEX], VISIBILITY_VERTEX);
	reflection_->Create(blob_->GetGraphicsBlobs()[GRAPHICS_PIXEL], VISIBILITY_PIXEL);

	reflection_->Bind("gCamera", MyEngine::camera3D->GetGPUVirtualAddress());
	reflection_->Bind("gParticle", particleBuffer_->GetGPUVirtualAddress());

	{
		pipeline_ = std::make_unique<GraphicsPipeline>();

		pipeline_->CreateRootSignature(MyEngine::GetDevicesObj(), reflection_->CreateRootSignatureDesc());

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

	auto commandList = MyEngine::GetCommandList();

	pipeline_->SetPipeline(commandList);
	plane_.SetBuffer(commandList);

	reflection_->Bind("gCamera", MyEngine::camera3D->GetGPUVirtualAddress()); //!< camera3Dは呼び出されるたび変わるので
	reflection_->BindGraphicsParameter(commandList);

	plane_.DrawCall(commandList, kParticleNum);
}

void Particle::SetAttributeImGui() {

	ImGui::Text("frequencyTime: %f", (*emitterBuffer_)[0].frequencyTime);

}
