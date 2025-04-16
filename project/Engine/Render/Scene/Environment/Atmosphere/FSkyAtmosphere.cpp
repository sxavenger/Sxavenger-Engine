#include "FSkyAtmosphere.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AtmosphereMap structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSkyAtmosphere::AtmosphereMap::Create(const Vector2ui& _size) {
	CreateBuffer(_size);
	CreatePipeline();
}

void FSkyAtmosphere::AtmosphereMap::Dispatch(const DirectXThreadContext* context) {

	pipeline->SetPipeline(context->GetDxCommand());

	BindBufferDesc desc = {};
	desc.SetHandle("gAtmosphere", descriptorUAV.GetGPUHandle());
	pipeline->BindComputeBuffer(context->GetDxCommand(), desc);

	Vector3ui threadGroup = { RoundUp(size.x, 16), RoundUp(size.y, 16), 6 };
	pipeline->Dispatch(context->GetDxCommand(), threadGroup);

}

void FSkyAtmosphere::AtmosphereMap::CreateBuffer(const Vector2ui& _size) {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = _size.x;
		desc.Height           = _size.y;
		desc.DepthOrArraySize = kCubemap_;
		desc.MipLevels        = 1;
		desc.Format           = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		// resourceの生成
		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&resource)
		);
		Assert(SUCCEEDED(hr), "atmosphere map create failed.");

	}

	{ //!< SRVの生成

		// handleの取得
		descriptorSRV = SxavengerSystem::GetDescriptor(kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURECUBE;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.TextureCube.MipLevels   = 1;

		// SRVの生成
		device->CreateShaderResourceView(
			resource.Get(),
			&desc,
			descriptorSRV.GetCPUHandle()
		);
	}

	{ //!< UAVの生成

		// handleの取得
		descriptorUAV = SxavengerSystem::GetDescriptor(kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format                   = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension            = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = kCubemap_;

		// UAVの生成
		device->CreateUnorderedAccessView(
			resource.Get(),
			nullptr,
			&desc,
			descriptorUAV.GetCPUHandle()
		);
	}

	//* parameterの保存
	size = _size;
}

void FSkyAtmosphere::AtmosphereMap::CreatePipeline() {

	pipeline = std::make_unique<DxObject::ReflectionComputePipelineState>();
	pipeline->CreateBlob(kPackagesShaderDirectory / "render/atmosphere/skyAtmosphere.cs.hlsl");
	pipeline->ReflectionPipeline(SxavengerSystem::GetDxDevice());

}

////////////////////////////////////////////////////////////////////////////////////////////
// FSkyAtmosphere class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSkyAtmosphere::Create(const Vector2ui& size) {
	atmosphere_.Create(size);
	FEnvironmentMap::Create(size);
}

void FSkyAtmosphere::Update(const DirectXThreadContext* context) {
	atmosphere_.Dispatch(context);
	FEnvironmentMap::Dispatch(context, atmosphere_.descriptorSRV.GetGPUHandle());
}

