#include "DirectXRCommon.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <d3dx12.h>
#include <MyEngine.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxrObject;

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXRCommon class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectXRCommon::InitRayTracing(int32_t clientWidth, int32_t clientHeight) {
	shaderManager_ = std::make_unique<DxrObject::ShaderManager>();
	
	resultBuffer_ = std::make_unique<DxrObject::ResultBuffer>();
	resultBuffer_->Init(
		devices_.get(), descriptorHeaps_.get(),
		clientWidth, clientHeight
	);

	CreateAccelerationStructure();
	CreateStateObject(clientWidth, clientHeight);

	camera_ = std::make_unique<RayTracingCamera>();
	light_ = std::make_unique<RayTracingLight>();
}

void DirectXRCommon::TermRayTracing() {
	camera_.reset();
	light_.reset();

	material_.reset();

	tlas_.reset();

	teapot_.reset();
	teapotBlas_.reset();

	roomBlas_.reset();
	room_.reset();

	shaderTable_.reset();
	stateObject_.reset();
	globalRootSignature_.reset();
	resultBuffer_.reset();
	shaderManager_.reset();
}



void DirectXRCommon::DrawRayTracing() { // HACK: ユーザーが使えるようにする

	auto commandList = command_->GetCommandList();

	// begin
	{
		// texture状態からuav状態に変更
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = resultBuffer_->GetResource();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

		commandList->ResourceBarrier(1, &barrier);
	}

	// process
	commandList->SetComputeRootSignature(globalRootSignature_->GetRootSignature());
	commandList->SetComputeRootDescriptorTable(0, tlas_->GetGPUDescriptorHandle());
	commandList->SetComputeRootConstantBufferView(1, camera_->GetGPUVirtualAddress());
	commandList->SetComputeRootConstantBufferView(2, light_->GetGPUVirtualAddress());

	commandList->SetPipelineState1(stateObject_->GetStateObject());
	commandList->DispatchRays(shaderTable_->GetDispatchRayDesc());

	// end
	{
		// uav結果をTextureとして使える状態にする
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource   = resultBuffer_->GetResource();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

		commandList->ResourceBarrier(1, &barrier);
	}
}

void DirectXRCommon::DrawRasterlize() {
	MyEngine::SetBlendMode(kBlendModeNormal);
	MyEngine::SetPipelineType(PRIMITIVE);
	MyEngine::SetPipelineState();

	auto commandList = command_->GetCommandList();

	{
		room_->SetBuffers(commandList, 0);

		commandList->SetGraphicsRootConstantBufferView(0, MyEngine::camera3D_->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(1, material_->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(2, light_->GetGPUVirtualAddress());

		room_->DrawCall(commandList, 0, 1);
	}

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

		globalRootSignature_ = std::make_unique<RootSignature>();
		globalRootSignature_->Init(devices_.get(), desc);
		stateDesc.globalRootSignature = globalRootSignature_.get();
	}

	// loaclRootSignatureの設定
	stateDesc.CreateLocalRootSignature(2);

	{
		RootSignatureDesc raygenDesc = {};
		raygenDesc.Resize(1, 0);

		//!< output(u0)
		raygenDesc.SetUAV(0, 0);

		stateDesc.SetLocalRootSignatureDesc(0, devices_.get(), raygenDesc);
	}

	{
		RootSignatureDesc closestHitDesc = {};
		closestHitDesc.Resize(2, 0);

		//!< vertex(t1)
		closestHitDesc.SetSRV(0, 1);

		//!< index(t2)
		closestHitDesc.SetSRV(1, 2);

		stateDesc.SetLocalRootSignatureDesc(1, devices_.get(), closestHitDesc);
	}

	// shaderBlobの生成
	{
		stateDesc.CreateShadeBlob();
		stateDesc.blob->Init(
			L"raytracing.hlsl",
			L"mainRayGen", L"mainCHS", L"mainMS"
		);
	}

	stateObject_ = std::make_unique<DxrObject::StateObject>();
	stateObject_->Init(devices_.get(), stateDesc);

	shaderTable_ = std::make_unique<DxrObject::ShaderTable>(); // stateDescを使いたいのでここに入れとく
	shaderTable_->Init(
		clientWidth, clientHeight,
		stateDesc, tlas_.get(), stateObject_.get(), resultBuffer_.get()
	);

}

void DirectXRCommon::CreateAccelerationStructure() {

	room_ = std::make_unique<Model>("./Resources/model", "room.obj");

	roomBlas_ = std::make_unique<DxrObject::BottomLevelAS>();
	roomBlas_->Create(room_->GetMeshData(0).vertexResource.get(), room_->GetMeshData(0).indexResource.get());

	teapot_ = std::make_unique<Model>("./Resources/model", "teapot.obj");

	teapotBlas_ = std::make_unique<DxrObject::BottomLevelAS>();
	teapotBlas_->Create(teapot_->GetMeshData(0).vertexResource.get(), teapot_->GetMeshData(0).indexResource.get());

	InstanceDesc desc = {};
	desc.Resize(2);

	desc.Set(0, roomBlas_.get(), Matrix4x4::MakeIdentity(), 0, 0);

	desc.Set(1, teapotBlas_.get(), Matrix4x4::MakeIdentity(), 1, 0);

	tlas_ = std::make_unique<DxrObject::TopLevelAS>();
	tlas_->Create(desc);

	material_ = std::make_unique<DxObject::BufferResource<Vector4f>>(devices_.get(), 1);
	material_->operator[](0) = {1.0f, 1.0f, 1.0f, 1.0f};

}

