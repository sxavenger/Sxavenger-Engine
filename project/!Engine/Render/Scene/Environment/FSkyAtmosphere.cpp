#include "FSkyAtmosphere.h"
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSkyAtmosphere::Parameter::Init() {
	direction = { 0.0f, 1.0f, 0.0f };
	intensity = 20.0f;
}

void FSkyAtmosphere::Parameter::SetImGuiCommand() {
	if (SxImGui::DragVector3("direction", &direction.x, 0.01f)) {
		direction = direction.Normalize();
	}

	SxImGui::DragFloat("intensity", &intensity, 0.1f, 0.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////
// AtmosphereMap structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSkyAtmosphere::AtmosphereMap::Create(const Vector2ui& _size) {
	//* parameterの保存
	size = _size;

	CreateBuffer();
	CreatePipeline();
	CreateDimensionBuffer();
}

void FSkyAtmosphere::AtmosphereMap::Dispatch(const DirectXQueueContext* context) {

	pipeline->SetPipeline(context->GetDxCommand());

	asyncResource.Transition(context->GetDxCommand(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	BindBufferDesc desc = {};
	desc.SetHandle("gAtmosphere", asyncDescriptorUAV.GetGPUHandle());
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());
	pipeline->BindComputeBuffer(context->GetDxCommand(), desc);

	Vector3ui threadGroup = { RoundUp(size.x, 16), RoundUp(size.y, 16), 6 };
	pipeline->Dispatch(context->GetDxCommand(), threadGroup);

	asyncResource.Transition(context->GetDxCommand(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

}

void FSkyAtmosphere::AtmosphereMap::Commit(const DirectXQueueContext* context) {
	// async resourceの状態を変更
	asyncResource.Transition(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);

	// main resourceの状態を変更
	mainResource.Transition(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_COPY_DEST
	);

	context->GetCommandList()->CopyResource(
		mainResource.Get(),
		asyncResource.Get()
	);
}

const DxObject::Descriptor& FSkyAtmosphere::AtmosphereMap::UseDescriptorSRV(const DirectXQueueContext* context) {
	// SRVを使える状態に遷移
	mainResource.Transition(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE
	);

	return mainDescriptorSRV;
}

void FSkyAtmosphere::AtmosphereMap::CreateBuffer() {


	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	{ //!< async resourceの生成
		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = size.x;
		desc.Height           = size.y;
		desc.DepthOrArraySize = kCubemap_;
		desc.MipLevels        = 1;
		desc.Format           = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		// resourceの生成
		asyncResource = DxObject::ResourceStateTracker::CreateCommittedResource(
			SxavengerSystem::GetDxDevice(),
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
			nullptr
		);
		asyncResource.SetName(L"AtmosphereMap::asyncResource");
	}

	
	{ //!< async SRVの生成

		// handleの取得
		asyncDescriptorSRV = SxavengerSystem::GetDescriptor(kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURECUBE;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.TextureCube.MipLevels   = 1;

		// SRVの生成
		device->CreateShaderResourceView(
			asyncResource.Get(),
			&desc,
			asyncDescriptorSRV.GetCPUHandle()
		);
	}

	{ //!< async UAVの生成

		// handleの取得
		asyncDescriptorUAV = SxavengerSystem::GetDescriptor(kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format                   = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.ViewDimension            = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = kCubemap_;

		// UAVの生成
		device->CreateUnorderedAccessView(
			asyncResource.Get(),
			nullptr,
			&desc,
			asyncDescriptorUAV.GetCPUHandle()
		);
	}

	{ //!< main resourceの生成
		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = size.x;
		desc.Height           = size.y;
		desc.DepthOrArraySize = kCubemap_;
		desc.MipLevels        = 1;
		desc.Format           = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		// resourceの生成
		mainResource = DxObject::ResourceStateTracker::CreateCommittedResource(
			SxavengerSystem::GetDxDevice(),
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
			nullptr
		);
		mainResource.SetName(L"AtmosphereMap::mainResource");
	}

	{ //!< main SRVの生成
		// handleの取得
		mainDescriptorSRV = SxavengerSystem::GetDescriptor(kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = DXGI_FORMAT_R11G11B10_FLOAT;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURECUBE;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.TextureCube.MipLevels   = 1;

		// SRVの生成
		device->CreateShaderResourceView(
			mainResource.Get(),
			&desc,
			mainDescriptorSRV.GetCPUHandle()
		);
	}

}

void FSkyAtmosphere::AtmosphereMap::CreatePipeline() {

	pipeline = std::make_unique<DxObject::ReflectionComputePipelineState>();
	pipeline->CreateBlob(kPackagesShaderDirectory / "render/atmosphere/skyAtmosphere.cs.hlsl");
	pipeline->ReflectionPipeline(SxavengerSystem::GetDxDevice());

}

void FSkyAtmosphere::AtmosphereMap::CreateDimensionBuffer() {
	parameter_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

////////////////////////////////////////////////////////////////////////////////////////////
// FSkyAtmosphere class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSkyAtmosphere::Create(const Vector2ui& size) {
	atmosphere_.Create(size);
	FEnvironmentMap::Create(size);

	task_->SetTag("atmosphere and prefilterd map");
}

void FSkyAtmosphere::Update(const DirectXQueueContext* context) {
	if (!task_->IsCompleted()) {
		return;
	}

	// irrandiance, radince を main thread で使えるようにcopy.
	atmosphere_.Commit(context);
	irradiance_.Commit(context);
	radiance_.Commit(context);
	context->TransitionAllocator();

	task_->SetStatus(AsyncTask::Status::None);
	SxavengerSystem::PushTask(AsyncExecution::Compute, task_);
}

void FSkyAtmosphere::Task(const DirectXQueueContext* context) {
	atmosphere_.Dispatch(context);
	context->TransitionAllocator();

	mapEnvironment_ = atmosphere_.asyncDescriptorSRV.GetGPUHandle();
	FEnvironmentMap::Task(context);
}

void FSkyAtmosphere::SetImGuiCommand() {
	atmosphere_.parameter_->At(0).SetImGuiCommand();
}

const DxObject::Descriptor& FSkyAtmosphere::UseAtmosphereDescriptor(const DirectXQueueContext* context) {
	return atmosphere_.UseDescriptorSRV(context);
}

