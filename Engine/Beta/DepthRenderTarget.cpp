#include "DepthRenderTarget.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DepthRenderTarget class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DepthRenderTarget::Create(
	const Vector2ui& size, const Color4f& clearColor, DXGI_FORMAT format) {

	// 外部変数の保存
	descriptorHeaps_ = Sxavenger::GetDxCommon()->GetDescriptorsObj();
	size_            = size;
	clearColor_      = clearColor;
	format_          = format;

	CreateRenderTarget();
	CreateDepthStencil();

}

void DepthRenderTarget::Term() {
	descriptorHeaps_->DeleteDescriptor(descriptorRTV_);
	descriptorHeaps_->DeleteDescriptor(descriptorSRV_);
	descriptorHeaps_->DeleteDescriptor(descriptorDSV_);
	descriptorHeaps_ = nullptr;
}

void DepthRenderTarget::CreateRenderTarget() {

	auto device = Sxavenger::GetDevice();

	// resourceの生成
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = size_.x;
		desc.Height           = size_.y;
		desc.MipLevels        = 1;
		desc.Format           = format_;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		desc.SampleDesc.Count = 1;
		desc.DepthOrArraySize = 1;

		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format   = format_;
		clearValue.Color[0] = clearColor_.r;
		clearValue.Color[1] = clearColor_.g;
		clearValue.Color[2] = clearColor_.b;
		clearValue.Color[3] = clearColor_.a;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(&resource_)
		);

		Assert(SUCCEEDED(hr));
	}

	// SRV - shaderResourceViewの生成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = format_;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels     = 1;

		// SRVを生成するDescriptorHeapの場所を決める
		descriptorSRV_ = descriptorHeaps_->GetDescriptor(CBV_SRV_UAV);

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

	// RTV - RenderTargetViewの生成
	{
		descriptorRTV_ = descriptorHeaps_->GetDescriptor(RTV);

		device->CreateRenderTargetView(
			resource_.Get(),
			nullptr,
			descriptorRTV_.GetCPUHandle()
		);
	}
}

void DepthRenderTarget::CreateDepthStencil() {

	auto device = Sxavenger::GetDevice();

	// resourceの生成
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = size_.x;
		desc.Height           = size_.y;
		desc.MipLevels        = 1;
		desc.Format           = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		desc.SampleDesc.Count = 1;
		desc.DepthOrArraySize = 1;

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// 深度値のクリア設定
		D3D12_CLEAR_VALUE clear = {};
		clear.DepthStencil.Depth = 1.0f;
		clear.Format             = desc.Format;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clear,
			IID_PPV_ARGS(&resourceDSV_)
		);

		Assert(SUCCEEDED(hr));
	}

	// depthStensilViewの生成
	{
		// handleの取得
		descriptorDSV_ = descriptorHeaps_->GetDescriptor(DSV);

		// descの設定
		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format        = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

		device->CreateDepthStencilView(
			resourceDSV_.Get(),
			&desc,
			descriptorDSV_.GetCPUHandle()
		);
	}
}
