#include "DirectXRCommon.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <d3dx12.h>
#include <MyEngine.h>
#include <Logger.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxrObject;

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXRCommon class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectXRCommon::InitRayTracing(int32_t clientWidth, int32_t clientHeight) {

	// dxr対応かどうか確認
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 option = {};

		auto hr = devices_->GetDevice()->CheckFeatureSupport(
			D3D12_FEATURE_D3D12_OPTIONS5,
			&option, sizeof(option)
		);

		assert(SUCCEEDED(hr));
		assert(option.RaytracingTier >= D3D12_RAYTRACING_TIER_1_0);

		Log("-- DXR version is compatible.");
	}

	// shaderManagerの生成
	shaderManager_ = std::make_unique<DxrObject::ShaderManager>();
	
	// resultBufferの生成
	resultBuffer_ = std::make_unique<DxrObject::ResultBuffer>();
	resultBuffer_->Init(
		devices_.get(), descriptorHeaps_.get(),
		clientWidth, clientHeight
	);

	// stateObjectの生成
	CreateStateObject(clientWidth, clientHeight);

	// recordBufferの生成
	CreateRecordBuffer();
}

void DirectXRCommon::TermRayTracing() {

	shaderTable_.reset();
	stateObject_.reset();
	globalRootSignature_.reset();
	resultBuffer_.reset();
	shaderManager_.reset();
}

void DirectXRCommon::BeginRayTracing(DxrObject::TopLevelAS* tlas) {

	// shaderTableの書き込み
	shaderTable_->Record(
		tlas, stateObject_.get(),
		raygenerationRecordBuffer_.get(), missRecordBuffer_.get()
	);

	auto commandList = command_->GetCommandList();

	// texture状態からuav状態に変更
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = resultBuffer_->GetResource();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	commandList->ResourceBarrier(1, &barrier);

	// rootSignatureの設定
	commandList->SetPipelineState1(stateObject_->GetStateObject());
	commandList->SetComputeRootSignature(globalRootSignature_->GetRootSignature());
}

void DirectXRCommon::EndRayTracing() {

	auto commandList = command_->GetCommandList();

	// stateObjectの設定とDispatchRays
	commandList->DispatchRays(shaderTable_->GetDispatchRayDesc());

	// uav結果をTextureとして使える状態にする
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = resultBuffer_->GetResource();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	commandList->ResourceBarrier(1, &barrier);

}

DirectXRCommon* DirectXRCommon::GetInstance() {
	static DirectXRCommon instance;
	return &instance;
}

void DirectXRCommon::CreateStateObject(int32_t clientWidth, int32_t clientHeight) {
	StateObjectDesc stateDesc = {};

	// GloabalRootSignatiureの設定
	{
		RootSignatureDesc desc = {};
		desc.Resize(3, 0);

		//!< tlas(t0)
		desc.SetSRV(0, 0);

		//!< Camera(b0)
		desc.SetCBV(1, 0);

		//!< light(b1)
		desc.SetCBV(2, 1);

		////!< groundTexture(t4)
		////!< sampler(s0)
		//desc.SetSRV(3, 4);
		//desc.SetSampler(0, TextureMode::MODE_MIRROR, 0);

		globalRootSignature_ = std::make_unique<RootSignature>();
		globalRootSignature_->Init(devices_.get(), desc);
		stateDesc.globalRootSignature = globalRootSignature_.get();
	}

	// loaclRootSignatureの設定

	{
		RootSignatureDesc raygenDesc = {};
		raygenDesc.Resize(1, 0);
		raygenDesc.SetLocalRootSignature(RAYGENERATION, L"mainRayGen");

		//!< output(u0)
		raygenDesc.SetUAV(0, 0);

		stateDesc.SetLocalRootSignatureDesc(0, devices_.get(), raygenDesc);
	}

	{
		RootSignatureDesc closestHitDesc = {};
		closestHitDesc.Resize(3, 1);
		closestHitDesc.SetLocalRootSignature(HITGROUP, L"ground");

		//!< index(t1)
		closestHitDesc.SetSRV(0, 1);

		//!< vertex(t2)
		closestHitDesc.SetSRV(1, 2);

		//!< groundTexture(t4)
		//!< sampler(s0)
		closestHitDesc.SetSRV(2, 4);
		closestHitDesc.SetSampler(0, TextureMode::MODE_MIRROR, 0);

		stateDesc.SetLocalRootSignatureDesc(1, devices_.get(), closestHitDesc);
	}

	{
		RootSignatureDesc closestHitDesc = {};
		closestHitDesc.Resize(2, 0);
		closestHitDesc.SetLocalRootSignature(HITGROUP, L"player");

		//!< index(t1)
		closestHitDesc.SetSRV(0, 1);

		//!< vertex(t2)
		closestHitDesc.SetSRV(1, 2);

		stateDesc.SetLocalRootSignatureDesc(2, devices_.get(), closestHitDesc);
	}

	{
		RootSignatureDesc closestHitDesc = {};
		closestHitDesc.Resize(2, 0);
		closestHitDesc.SetLocalRootSignature(HITGROUP, L"teapot");

		//!< index(t1)
		closestHitDesc.SetSRV(0, 1);

		//!< vertex(t2)
		closestHitDesc.SetSRV(1, 2);

		stateDesc.SetLocalRootSignatureDesc(3, devices_.get(), closestHitDesc);
	}

	{
		RootSignatureDesc closestHitDesc = {};
		closestHitDesc.Resize(2, 0);
		closestHitDesc.SetLocalRootSignature(HITGROUP, L"teapot");

		//!< index(t1)
		closestHitDesc.SetSRV(0, 1);

		//!< vertex(t2)
		closestHitDesc.SetSRV(1, 2);

		stateDesc.SetLocalRootSignatureDesc(4, devices_.get(), closestHitDesc);
	}

	// shaderBlobの生成
	{
		stateDesc.CreateShadeBlob();
		
		stateDesc.blob->Create(L"RayTracingWorld.hlsl");
		stateDesc.blob->SetShader(L"mainRayGen", DxrObject::ShaderType::RAYGENERATION_SHADER);
		stateDesc.blob->SetShader(L"mainMS", DxrObject::ShaderType::MISS_SHADER);

		stateDesc.blob->SetShader(L"mainGroundCHS", DxrObject::ShaderType::CLOSESTHIT_SHADER, L"ground");
		stateDesc.blob->SetShader(L"mainPlayerCHS", DxrObject::ShaderType::CLOSESTHIT_SHADER, L"player");
		stateDesc.blob->SetShader(L"mainTeapotCHS", DxrObject::ShaderType::CLOSESTHIT_SHADER, L"teapot");
		stateDesc.blob->SetShader(L"mainCubeCHS", DxrObject::ShaderType::CLOSESTHIT_SHADER, L"cube");
	}

	stateObject_ = std::make_unique<DxrObject::StateObject>();
	stateObject_->Init(devices_.get(), stateDesc);

	shaderTable_ = std::make_unique<DxrObject::ShaderTable>();
	shaderTable_->Init(clientWidth, clientHeight);
}

void DirectXRCommon::CreateRecordBuffer() {

	// raygeneration
	raygenerationRecordBuffer_ = std::make_unique<RecordBuffer>();

	raygenerationRecordBuffer_->SetExport(ExportType::RAYGENERATION, L"mainRayGen");
	raygenerationRecordBuffer_->SetBuffer(0, resultBuffer_->GetDescriptorUAV().handleGPU);

	// miss
	missRecordBuffer_ = std::make_unique<RecordBuffer>();

	missRecordBuffer_->SetExport(ExportType::RAYGENERATION, L"mainMS");

}
