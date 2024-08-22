#include "DxDepthStencil.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include "DxDevices.h"
#include "DxDescriptorHeaps.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DepthStencil methods
////////////////////////////////////////////////////////////////////////////////////////////

DxObject::DepthStencil::DepthStencil(
	Devices* devices, DescriptorHeaps* descriptorHeaps,
	int32_t clientWidth, int32_t clientHeight) {

	Init(devices, descriptorHeaps, clientWidth, clientHeight);
}

DxObject::DepthStencil::~DepthStencil() { Term(); }

void DxObject::DepthStencil::Init(
	Devices* devices, DescriptorHeaps* descriptorHeaps,
	int32_t clientWidth, int32_t clientHeight) {

	// デバイスの取り出し
	ID3D12Device* device = devices->GetDevice();

	// resourceの生成
	{
		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = clientWidth;
		desc.Height           = clientHeight;
		desc.MipLevels        = 1;
		desc.DepthOrArraySize = 1;
		desc.Format           = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

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
			IID_PPV_ARGS(&resource_)
		);

		assert(SUCCEEDED(hr));
	}

	// depthStensilViewの生成
	{
		// handleの取得
		descriptorDSV_ = descriptorHeaps->GetDescriptor(DescriptorType::DSV);

		// descの設定
		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format        = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

		device->CreateDepthStencilView(
			resource_.Get(),
			&desc,
			descriptorDSV_.GetCPUHandle()
		);
	}
}

void DxObject::DepthStencil::Term() {
}