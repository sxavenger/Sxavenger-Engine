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

	// depthStensilViewの生成
	{
		depthStencilResource_ = DxObjectMethod::CreateDepthStencilTextureResource(
			device,
			clientWidth,
			clientHeight
		);

		// handleの取得
		descriptorDSV_ = descriptorHeaps->GetCurrentDescriptor(DescriptorType::DSV);

		// descの設定
		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format        = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

		device->CreateDepthStencilView(
			depthStencilResource_.Get(),
			&desc,
			descriptorDSV_.GetCPUHandle()
		);
	}
}

void DxObject::DepthStencil::Term() {
	depthStencilResource_->Release();
}