#include "FSkyAtmosphere.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FSkyAtmosphere class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSkyAtmosphere::Create(const Vector2ui& size) {

	size_ = size;

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = size.x;
		desc.Height           = size.y;
		desc.DepthOrArraySize = 6; //!< cubemap
		desc.MipLevels        = 1;
		desc.Format           = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		// resourceの生成
		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&cubemap_)
		);
		Assert(SUCCEEDED(hr), "sky atmosphere cubemap is create failed.");

	}

	{ //!< SRVの生成

		// handleの取得
		cubemapSRV_ = SxavengerSystem::GetDescriptor(kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                            = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension                     = D3D12_SRV_DIMENSION_TEXTURECUBE;
		desc.Shader4ComponentMapping           = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.TextureCube.MipLevels             = 1;

		// SRVの生成
		device->CreateShaderResourceView(
			cubemap_.Get(),
			&desc,
			cubemapSRV_.GetCPUHandle()
		);
	}

	{ //!< UAVの生成

		// handleの取得
		cubemapUAV_ = SxavengerSystem::GetDescriptor(kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format                            = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension                     = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize          = 6; //!< cubemap

		// UAVの生成
		device->CreateUnorderedAccessView(
			cubemap_.Get(),
			nullptr,
			&desc,
			cubemapUAV_.GetCPUHandle()
		);
	}

	CreatePipeline(); //!< Test
}

void FSkyAtmosphere::Dispatch(const DirectXThreadContext* context) {

	pipeline_->SetPipeline(context->GetDxCommand());

	BindBufferDesc desc = {};
	desc.SetHandle("gCube", cubemapUAV_.GetGPUHandle());

	pipeline_->BindComputeBuffer(context->GetDxCommand(), desc);

	Vector3ui threadGroup = { RoundUp(size_.x, 16), RoundUp(size_.y, 16), 6 };

	pipeline_->Dispatch(context->GetDxCommand(), threadGroup);
}

void FSkyAtmosphere::CreatePipeline() {

	pipeline_ = std::make_unique<DxObject::ReflectionComputePipelineState>();
	pipeline_->CreateBlob(kPackagesShaderDirectory / "render/atmosphere/skyAtmosphere.cs.hlsl");
	pipeline_->ReflectionPipeline(SxavengerSystem::GetDxDevice());



}
