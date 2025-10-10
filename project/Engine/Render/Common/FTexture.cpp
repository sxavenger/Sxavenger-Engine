#include "FTexture.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FTexture::Create(const Vector2ui& size, DXGI_FORMAT format) {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	size_   = size;
	format_ = format;

	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = size_.x;
		desc.Height           = size_.y;
		desc.MipLevels        = 1;
		desc.DepthOrArraySize = 1;
		desc.Format           = format_;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		// clearValueの設定
		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = format_;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			kDefaultState_,
			&clearValue,
			IID_PPV_ARGS(&resource_)
		);
		DxObject::Assert(hr, L"texture create failed.");


		resource_->SetName(L"FTexture");
	}

	{ //!< RTVの生成

		// handleの取得
		descriptorRTV_ = SxavengerSystem::GetDescriptor(kDescriptor_RTV);

		// descの設定
		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format        = format_;
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		// RTVの生成
		device->CreateRenderTargetView(
			resource_.Get(),
			&desc,
			descriptorRTV_.GetCPUHandle()
		);
	}

	{ //!< UAVの生成

		// handleの取得
		descriptorUAV_ = SxavengerSystem::GetDescriptor(kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format        = format_;
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

		// UAVの生成
		device->CreateUnorderedAccessView(
			resource_.Get(),
			nullptr,
			&desc,
			descriptorUAV_.GetCPUHandle()
		);
	}

	{ //!< SRVの生成

		// handleの取得
		descriptorSRV_ = SxavengerSystem::GetDescriptor(kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                    = format_;
		desc.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Shader4ComponentMapping   = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Texture2D.MipLevels       = 1;

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}
}
