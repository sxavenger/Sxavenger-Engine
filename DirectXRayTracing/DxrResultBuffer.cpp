#include "DxrResultBuffer.h"

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// ResultBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::ResultBuffer::Init(
	DxObject::Devices* devices, DxObject::DescriptorHeaps* descriptorHeaps,
	int32_t clientWidth, int32_t clientHeight) {

	descriptorHeaps_ = descriptorHeaps;

	// デバイスの取り出し
	auto device = devices->GetDevice();

	// resourceの生成
	{
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment        = 0;
		desc.Width            = clientWidth;
		desc.Height           = clientHeight;
		desc.DepthOrArraySize = 1;
		desc.MipLevels        = 1;
		desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Layout           = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&resource_)
		);

		assert(SUCCEEDED(hr));
	}

	// UAVの生成
	{
		descriptorUAV_ = descriptorHeaps_->GetCurrentDescriptor(UAV);

		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		desc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM;

		device->CreateUnorderedAccessView(
			resource_.Get(),
			nullptr,
			&desc,
			descriptorUAV_.handleCPU
		);
	}

	// SRVの生成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels     = 1;

		// SRVを生成するDescriptorHeapの場所を決める
		descriptorSRV_ = descriptorHeaps_->GetCurrentDescriptor(SRV);

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.handleCPU
		);
	}
}

void DxrObject::ResultBuffer::Term() {
	descriptorHeaps_->Erase(descriptorUAV_);
	descriptorHeaps_->Erase(descriptorSRV_);
	descriptorHeaps_ = nullptr;
}
