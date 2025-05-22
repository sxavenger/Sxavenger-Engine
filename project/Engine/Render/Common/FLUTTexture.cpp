#include "FLUTTexture.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FLUTTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FLUTTexture::Create(const AssetObserver<AssetTexture>& texture, const Vector2ui& tile) {

	// 引数の保存
	texture_ = texture.WaitGet();

	CreateResource(texture_->GetSize(), tile);
	CreateBuffer(texture_->GetSize(), tile);
}

void FLUTTexture::Dispatch(const DirectXThreadContext* context) {

	FRenderCore::GetInstance()->GetProcess()->SetPipeline(
		FRenderCoreProcess::CompositeType::ConvertLUTTexture, context
	);

	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());
	parameter.SetHandle("gInput",      texture_->GetGPUHandleSRV());
	parameter.SetHandle("gOutput",     descriptorUAV_.GetGPUHandle());

	FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(
		FRenderCoreProcess::CompositeType::ConvertLUTTexture, context, parameter
	);

	FRenderCore::GetInstance()->GetProcess()->Dispatch(context, texture_->GetSize());
}

void FLUTTexture::CreateResource(const Vector2ui& size, const Vector2ui& tile) {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();
	
	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = tile.x;
		desc.Height           = tile.y;
		desc.DepthOrArraySize = static_cast<UINT16>(size.x / tile.x * size.y / tile.y);
		desc.MipLevels        = 1;
		desc.Format           = DXGI_FORMAT_R10G10B10A2_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&resource_)
		);
		Assert(SUCCEEDED(hr), "lut texture create failed.");

		resource_->SetName(L"FLUTTexture");
	}

	{ //!< SRVの生成

		// descriptorの取得
		descriptorSRV_ = SxavengerSystem::GetDescriptor(kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Format                  = DXGI_FORMAT_R10G10B10A2_UNORM;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE3D;
		desc.Texture3D.MipLevels     = 1;

		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

	{ //!< UAVの生成

		// descriptorの取得
		descriptorUAV_ = SxavengerSystem::GetDescriptor(kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format          = DXGI_FORMAT_R10G10B10A2_UNORM;
		desc.ViewDimension   = D3D12_UAV_DIMENSION_TEXTURE3D;
		desc.Texture3D.WSize = size.x / tile.x * size.y / tile.y;

		device->CreateUnorderedAccessView(
			resource_.Get(),
			nullptr,
			&desc,
			descriptorUAV_.GetCPUHandle()
		);
	}
}

void FLUTTexture::CreateBuffer(const Vector2ui& size, const Vector2ui& tile) {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).size = size;
	parameter_->At(0).tile = tile;
}

