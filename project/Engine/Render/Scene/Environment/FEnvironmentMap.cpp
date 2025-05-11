#include "FEnvironmentMap.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// IrradianceMap structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void FEnvironmentMap::IrradianceMap::Create(const Vector2ui& _size) {
	// 引数の保存
	size = _size;

	CreateBuffer();
	CreatePipeline();
}

void FEnvironmentMap::IrradianceMap::Dispatch(const DirectXThreadContext* context) {
	if (!environment_.has_value()) {
		return;
	}

	asyncResource.TransitionToExpectedState(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	pipeline->SetPipeline(context->GetDxCommand());

	BindBufferDesc desc = {};
	desc.SetHandle("gEnvironment", environment_.value());
	desc.SetHandle("gIrrandiance", asyncDescriptorUAV.GetGPUHandle());
	pipeline->BindComputeBuffer(context->GetDxCommand(), desc);

	Vector3ui threadGroup = { RoundUp(size.x, kNumThreads_.x), RoundUp(size.y, kNumThreads_.y), 6 };
	pipeline->Dispatch(context->GetDxCommand(), threadGroup);

}

void FEnvironmentMap::IrradianceMap::Commit() {
	// 条件: Dispatch(...)と同時に呼び出されないように調整する.

	if (!environment_.has_value()) {
		return;
	}

	// HACK: main threadでの実行
	auto context = SxavengerSystem::GetMainThreadContext();

	// async resourceの状態を変更
	asyncResource.TransitionToExpectedState(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);

	// main resourceの状態を変更
	mainResource.TransitionToExpectedState(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_COPY_DEST
	);

	context->GetCommandList()->CopyResource(
		mainResource.Get(),
		asyncResource.Get()
	);

	//std::shared_ptr<AsyncTask> copyTask = std::make_shared<AsyncTask>();
	//copyTask->SetTag("IrradianceMap::Commit");
	//copyTask->SetFunction([this](const AsyncThread* thread) {

	//	auto context = thread->RequireContext();
	//	
	//	// async resourceの状態を変更
	//	asyncResource.TransitionToExpectedState(
	//		context->GetDxCommand(),
	//		D3D12_RESOURCE_STATE_COPY_SOURCE
	//	);

	//	// main resourceの状態を変更
	//	mainResource.TransitionToExpectedState(
	//		context->GetDxCommand(),
	//		D3D12_RESOURCE_STATE_COPY_DEST
	//	);

	//	context->GetCommandList()->CopyResource(
	//		mainResource.Get(),
	//		asyncResource.Get()
	//	);

	//});

	//SxavengerSystem::PushTask(AsyncExecution::Copy, copyTask);
	//copyTask->Wait();
}

const DxObject::Descriptor& FEnvironmentMap::IrradianceMap::UseDescriptorSRV(const DirectXThreadContext* context) {
	// SRVを使える状態に遷移
	mainResource.TransitionToExpectedState(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE
	);

	return mainDescriptorSRV;
}

void FEnvironmentMap::IrradianceMap::CreateBuffer() {

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
		desc.Format           = DXGI_FORMAT_R32G32B32A32_FLOAT;
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

		asyncResource.SetName(L"IrradianceMap::asyncResource");
	}

	{ //!< UAVの生成

		// handleの取得
		asyncDescriptorUAV = SxavengerSystem::GetDescriptor(kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format                   = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension            = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = kCubemap_;

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
		desc.Format           = DXGI_FORMAT_R32G32B32A32_FLOAT;
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

		mainResource.SetName(L"IrradianceMap::mainResource");
	}

	{ //!< SRVの生成

		// handleの取得
		mainDescriptorSRV = SxavengerSystem::GetDescriptor(kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = DXGI_FORMAT_R32G32B32A32_FLOAT;
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

void FEnvironmentMap::IrradianceMap::CreatePipeline() {
	pipeline = std::make_unique<DxObject::ReflectionComputePipelineState>();
	pipeline->CreateBlob(kPackagesShaderDirectory / "render/prefiltered/irradiance.cs.hlsl");
	pipeline->ReflectionPipeline(SxavengerSystem::GetDxDevice());
}

////////////////////////////////////////////////////////////////////////////////////////////
// RadianceMap structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void FEnvironmentMap::RadianceMap::Create(const Vector2ui& _size) {
	// 引数の保存
	size = _size;

	CreateBuffer();
	CreateDimensionBuffer();
	CreatePipeline();
}

void FEnvironmentMap::RadianceMap::Dispatch(const DirectXThreadContext* context) {
	if (!environment_.has_value()) {
		return;
	}

	asyncResource.TransitionToExpectedState(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	pipeline->SetPipeline(context->GetDxCommand());

	BindBufferDesc desc = {};
	desc.SetHandle("gEnvironment", environment_.value());
	desc.SetAddress("gIndices",    indices->GetGPUVirtualAddress());
	desc.SetAddress("gParameter",  parameter->GetGPUVirtualAddress());
	pipeline->BindComputeBuffer(context->GetDxCommand(), desc);

	Vector3ui threadGroup = { RoundUp(size.x, kNumThreads_.x), RoundUp(size.y, kNumThreads_.y), 6 * kMiplevels };
	pipeline->Dispatch(context->GetDxCommand(), threadGroup);

}

void FEnvironmentMap::RadianceMap::Commit() {
	// 条件: Dispatch(...)と同時に呼び出されないように調整する.

	if (!environment_.has_value()) {
		return;
	}

	// HACK: main threadでの実行
	auto context = SxavengerSystem::GetMainThreadContext();
	
	// async resourceの状態を変更
	asyncResource.TransitionToExpectedState(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);

	// main resourceの状態を変更
	mainResource.TransitionToExpectedState(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_COPY_DEST
	);

	context->GetCommandList()->CopyResource(
		mainResource.Get(),
		asyncResource.Get()
	);

	//std::shared_ptr<AsyncTask> copyTask = std::make_shared<AsyncTask>();
	//copyTask->SetTag("RadianceMap::Commit");
	//copyTask->SetFunction([this](const AsyncThread* thread) {

	//	auto context = thread->RequireContext();
	//	
	//	// async resourceの状態を変更
	//	asyncResource.TransitionToExpectedState(
	//		context->GetDxCommand(),
	//		D3D12_RESOURCE_STATE_COPY_SOURCE
	//	);

	//	// main resourceの状態を変更
	//	mainResource.TransitionToExpectedState(
	//		context->GetDxCommand(),
	//		D3D12_RESOURCE_STATE_COPY_DEST
	//	);

	//	context->GetCommandList()->CopyResource(
	//		mainResource.Get(),
	//		asyncResource.Get()
	//	);
	//});

	//SxavengerSystem::PushTask(AsyncExecution::Copy, copyTask);
	//copyTask->Wait();
}

const DxObject::Descriptor& FEnvironmentMap::RadianceMap::UseDescriptorSRV(const DirectXThreadContext* context) {
	// SRVを使える状態に遷移
	mainResource.TransitionToExpectedState(
		context->GetDxCommand(),
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE
	);

	return mainDescriptorSRV;
}

void FEnvironmentMap::RadianceMap::CreateBuffer() {
	
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
		desc.MipLevels        = kMiplevels;
		desc.Format           = DXGI_FORMAT_R32G32B32A32_FLOAT;
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

		asyncResource.SetName(L"RadianceMap::asyncResource");
	}

	{ //!< UAVの生成
		for (UINT16 i = 0; i < kMiplevels; ++i) {

			// handleの取得
			asyncDescriptorUAVs[i] = SxavengerSystem::GetDescriptor(kDescriptor_UAV);

			// descの設定
			D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
			desc.Format                   = DXGI_FORMAT_R32G32B32A32_FLOAT;
			desc.ViewDimension            = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.ArraySize = kCubemap_;
			desc.Texture2DArray.MipSlice  = i;

			// UAVの生成
			device->CreateUnorderedAccessView(
				asyncResource.Get(),
				nullptr,
				&desc,
				asyncDescriptorUAVs[i].GetCPUHandle()
			);
		}
	}

	{ //!< main resource

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = size.x;
		desc.Height           = size.y;
		desc.DepthOrArraySize = kCubemap_;
		desc.MipLevels        = kMiplevels;
		desc.Format           = DXGI_FORMAT_R32G32B32A32_FLOAT;
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

		mainResource.SetName(L"RadianceMap::mainResource");

	}

	{ //!< SRVの生成

		// handleの取得
		mainDescriptorSRV = SxavengerSystem::GetDescriptor(kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURECUBE;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.TextureCube.MipLevels   = kMiplevels;

		// SRVの生成
		device->CreateShaderResourceView(
			mainResource.Get(),
			&desc,
			mainDescriptorSRV.GetCPUHandle()
		);
	}
	
}

void FEnvironmentMap::RadianceMap::CreateDimensionBuffer() {

	auto device = SxavengerSystem::GetDxDevice();

	indices = std::make_unique<DxObject::DimensionBuffer<uint32_t>>();
	indices->Create(device, kMiplevels);

	for (UINT16 i = 0; i < kMiplevels; ++i) {
		indices->At(i) = asyncDescriptorUAVs[i].GetIndex();
	}

	parameter = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	parameter->Create(device, 1);
	parameter->At(0).size      = { size.x, size.y };
	parameter->At(0).miplevels = kMiplevels;
	
}

void FEnvironmentMap::RadianceMap::CreatePipeline() {
	pipeline = std::make_unique<DxObject::ReflectionComputePipelineState>();
	pipeline->CreateBlob(kPackagesShaderDirectory / "render/prefiltered/radiance.cs.hlsl");
	pipeline->ReflectionPipeline(SxavengerSystem::GetDxDevice());
}

////////////////////////////////////////////////////////////////////////////////////////////
// FEnvironmentMap class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FEnvironmentMap::Create(const Vector2ui& size) {
	irradiance_.Create(size);
	radiance_.Create(size);

	task_ = std::make_shared<AsyncTask>();
	task_->SetFunction([this](const AsyncThread* thread) {
		this->Task(thread->GetContext());
	});

	task_->SetStatus(AsyncTask::Status::Completed);
}

void FEnvironmentMap::Term() {
	task_->Wait();
}

void FEnvironmentMap::Update() {
	if (task_->IsCompleted()) {
		// irrandiance, radince を main thread で使えるようにcopy.
		if (!isCommited_) {
			irradiance_.Commit();
			radiance_.Commit();
			isCommited_ = true;
		}
		

		// 再度実行が必須か確認
		bool isNeedExecute = false;

		if (environment_.has_value()) {
			if (irradiance_.GetEnvironment().has_value()) {
				isNeedExecute = (environment_.value().ptr != irradiance_.GetEnvironment().value().ptr);

			} else {
				isNeedExecute = true;
			}
		}

		// 再度taskを実行
		if (isNeedExecute) {
			irradiance_.SetEnvironment(environment_);
			radiance_.SetEnvironment(environment_);

			task_->SetStatus(AsyncTask::Status::None);
			SxavengerSystem::PushTask(AsyncExecution::Compute, task_);

			isCommited_ = false;
		}
	}
}

void FEnvironmentMap::Task(const DirectXThreadContext* context) {
	if (!environment_.has_value()) {
		return;
	}

	irradiance_.Dispatch(context);
	radiance_.Dispatch(context);
}

const DxObject::Descriptor& FEnvironmentMap::UseIrradianceDescriptor(const DirectXThreadContext* context) {
	return irradiance_.UseDescriptorSRV(context);
}

const DxObject::Descriptor& FEnvironmentMap::UseRadianceDescriptor(const DirectXThreadContext* context) {
	return radiance_.UseDescriptorSRV(context);
}
