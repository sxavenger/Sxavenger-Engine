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
}

void DirectXRCommon::TermRayTracing() {

	shaderTable_.reset();
	stateObject_.reset();
	globalRootSignature_.reset();
	resultBuffer_.reset();
	shaderManager_.reset();
}

void DirectXRCommon::CreateStateObject(
	int32_t clientWidth, int32_t clientHeight, DxrObject::TopLevelAS* tlas) {
	
	StateObjectDesc stateDesc = {};

	// GloabalRootSignatiureの設定
	{
		RootSignatureDesc desc = {};
		desc.Resize(5, 1);

		//!< tlas(t0)
		desc.SetSRV(0, 0);

		//!< Camera(b0)
		desc.SetCBV(1, 0);

		//!< light(b1)
		desc.SetCBV(2, 1);

		//!< glassMaterial(t3)
		desc.SetSRV(3, 3);

		//!< groundTexture(t4)
		//!< sampler(s0)
		desc.SetSRV(4, 4);
		desc.SetSampler(0, TextureMode::MODE_MIRROR, 0);

		

		globalRootSignature_ = std::make_unique<RootSignature>();
		globalRootSignature_->Init(devices_.get(), desc);
		stateDesc.globalRootSignature = globalRootSignature_.get();
	}

	// loaclRootSignatureの設定
	stateDesc.CreateLocalRootSignature(8);

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
		closestHitDesc.Resize(2, 0);
		closestHitDesc.SetLocalRootSignature(HITGROUP, L"ground");

		//!< vertex(t1)
		closestHitDesc.SetSRV(0, 1);

		//!< index(t2)
		closestHitDesc.SetSRV(1, 2);

		stateDesc.SetLocalRootSignatureDesc(1, devices_.get(), closestHitDesc);
	}

	{
		RootSignatureDesc closestHitDesc = {};
		closestHitDesc.Resize(2, 0);
		closestHitDesc.SetLocalRootSignature(HITGROUP, L"teapot");

		//!< vertex(t1)
		closestHitDesc.SetSRV(0, 1);

		//!< index(t2)
		closestHitDesc.SetSRV(1, 2);

		stateDesc.SetLocalRootSignatureDesc(2, devices_.get(), closestHitDesc);
	}

	{
		RootSignatureDesc closestHitDesc = {};
		closestHitDesc.Resize(2, 0);
		closestHitDesc.SetLocalRootSignature(HITGROUP, L"glass");

		//!< vertex(t1)
		closestHitDesc.SetSRV(0, 1);

		//!< index(t2)
		closestHitDesc.SetSRV(1, 2);

		stateDesc.SetLocalRootSignatureDesc(3, devices_.get(), closestHitDesc);
	}

	{
		RootSignatureDesc closestHitDesc = {};
		closestHitDesc.Resize(2, 0);
		closestHitDesc.SetLocalRootSignature(HITGROUP, L"bunny");

		//!< vertex(t1)
		closestHitDesc.SetSRV(0, 1);

		//!< index(t2)
		closestHitDesc.SetSRV(1, 2);

		stateDesc.SetLocalRootSignatureDesc(4, devices_.get(), closestHitDesc);
	}

	{
		RootSignatureDesc closestHitDesc = {};
		closestHitDesc.Resize(2, 0);
		closestHitDesc.SetLocalRootSignature(HITGROUP, L"player");

		//!< vertex(t1)
		closestHitDesc.SetSRV(0, 1);

		//!< index(t2)
		closestHitDesc.SetSRV(1, 2);

		stateDesc.SetLocalRootSignatureDesc(5, devices_.get(), closestHitDesc);
	}

	{
		RootSignatureDesc closestHitDesc = {};
		closestHitDesc.Resize(2, 0);
		closestHitDesc.SetLocalRootSignature(HITGROUP, L"room");

		//!< vertex(t1)
		closestHitDesc.SetSRV(0, 1);

		//!< index(t2)
		closestHitDesc.SetSRV(1, 2);

		stateDesc.SetLocalRootSignatureDesc(6, devices_.get(), closestHitDesc);
	}

	{
		RootSignatureDesc closestHitDesc = {};
		closestHitDesc.Resize(2, 0);
		closestHitDesc.SetLocalRootSignature(HITGROUP, L"ocean");

		//!< vertex(t1)
		closestHitDesc.SetSRV(0, 1);

		//!< index(t2)
		closestHitDesc.SetSRV(1, 2);

		stateDesc.SetLocalRootSignatureDesc(7, devices_.get(), closestHitDesc);
	}

	// shaderBlobの生成
	{
		stateDesc.CreateShadeBlob();
		
		stateDesc.blob->Create(L"RayTracingWorld.hlsl");
		stateDesc.blob->SetShader(L"mainRayGen", DxrObject::ShaderType::RAYGENERATION_SHADER);
		stateDesc.blob->SetShader(L"mainMS", DxrObject::ShaderType::MISS_SHADER);

		stateDesc.blob->SetShader(L"mainGroundCHS", DxrObject::ShaderType::CLOSESTHIT_SHADER, L"ground");
		stateDesc.blob->SetShader(L"mainTeapotCHS", DxrObject::ShaderType::CLOSESTHIT_SHADER, L"teapot");
		stateDesc.blob->SetShader(L"mainGlassCHS", DxrObject::ShaderType::CLOSESTHIT_SHADER, L"glass");
		stateDesc.blob->SetShader(L"mainBunnyCHS", DxrObject::ShaderType::CLOSESTHIT_SHADER, L"bunny");
		stateDesc.blob->SetShader(L"mainPlayerCHS", DxrObject::ShaderType::CLOSESTHIT_SHADER, L"player");
		stateDesc.blob->SetShader(L"mainRoomCHS", DxrObject::ShaderType::CLOSESTHIT_SHADER, L"room");
		stateDesc.blob->SetShader(L"mainOceanCHS", DxrObject::ShaderType::CLOSESTHIT_SHADER, L"ocean");
	}

	stateObject_ = std::make_unique<DxrObject::StateObject>();
	stateObject_->Init(devices_.get(), stateDesc);

	shaderTable_ = std::make_unique<DxrObject::ShaderTable>(); // stateDescを使いたいのでここに入れとく
	shaderTable_->Init(// XXX: 一番危険
		clientWidth, clientHeight,
		stateDesc, tlas, stateObject_.get(), resultBuffer_.get()
	);

}



//void DirectXRCommon::DrawRayTracing() { // HACK: ユーザーが使えるようにする
//
//	auto commandList = command_->GetCommandList();
//
//	BeginRayTracing();
//
//	// process
//	commandList->SetComputeRootSignature(globalRootSignature_->GetRootSignature());
//	commandList->SetComputeRootDescriptorTable(0, tlas_->GetGPUDescriptorHandle());
//	commandList->SetComputeRootConstantBufferView(1, camera_->GetGPUVirtualAddress());
//	commandList->SetComputeRootDescriptorTable(2, materialStructuredBuffer_->GetGPUHandle());
//	commandList->SetComputeRootConstantBufferView(3, light_->GetGPUVirtualAddress());
//	/*commandList->SetComputeRootConstantBufferView(3, ambientOcclusion_->GetGPUVirtualAddress());*/
//
//	commandList->SetPipelineState1(stateObject_->GetStateObject());
//	commandList->DispatchRays(shaderTable_->GetDispatchRayDesc());
//
//	EndRayTracing();
//
//}

void DirectXRCommon::BeginRayTracing() {

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
	commandList->SetComputeRootSignature(globalRootSignature_->GetRootSignature());
}

void DirectXRCommon::EndRayTracing() {

	auto commandList = command_->GetCommandList();

	// stateObjectの設定とDispatchRays
	commandList->SetPipelineState1(stateObject_->GetStateObject());
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

//void DirectXRCommon::CreateAccelerationStructure() {
//
//	// モデルの生成
//	glassRoom_ = std::make_unique<Model>("./Resources/model", "glassRoom.obj");
//	uint32_t modelSize = glassRoom_->GetSize();
//
//	// blasの生成
//	glassRoomBlas_.resize(modelSize);
//
//	//!< 0番目だけhitgroupが違うので手動設定
//	glassRoomBlas_[0] = std::make_unique<DxrObject::BottomLevelAS>();
//	glassRoomBlas_[0]->Create(
//		glassRoom_->GetMeshData(0).vertexResource.get(), glassRoom_->GetMeshData(0).indexResource.get(),
//		kRoomHitGroup_
//	);
//
//	//!< 他は同じhitgroupなので自動設定
//	for (uint32_t i = 1; i < modelSize; ++i) {
//		glassRoomBlas_[i] = std::make_unique<DxrObject::BottomLevelAS>();
//		glassRoomBlas_[i]->Create(
//			glassRoom_->GetMeshData(i).vertexResource.get(), glassRoom_->GetMeshData(i).indexResource.get(),
//			kGlassHitGroup_
//		);
//	}
//
//	// TLASの生成
//	InstanceDesc desc = {};
//	desc.Resize(modelSize);
//
//	for (uint32_t i = 0; i < modelSize; ++i) {
//		uint32_t instanceId = i;
//
//		if (instanceId > 0) {
//			instanceId--;
//
//			// glassのmaterialをstructuredBufferとして送るので
//		}
//
//		desc.Set(i, glassRoomBlas_[i].get(), Matrix4x4::MakeIdentity(), i, instanceId);
//	}
//
//	tlas_ = std::make_unique<DxrObject::TopLevelAS>();
//	tlas_->Create(desc);
//
//}
