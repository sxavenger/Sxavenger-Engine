#include "DxDepthStencil.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// DepthSteincil class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DepthStencil::Init(
	Device* devices, DescriptorHeaps* descriptorHeaps,
	const Vector2ui& size, DXGI_FORMAT format) {

	CreateResource(devices, size, format);
	CreateDSV(devices, descriptorHeaps, format);
}

void DepthStencil::Term() {
}

void DepthStencil::CreateResource(Device* devices, const Vector2ui& size, DXGI_FORMAT format) {

		// deviceの取得
	auto device = devices->GetDevice();

	// propの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	// descの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Width            = size.x;
	desc.Height           = size.y;
	desc.MipLevels        = 1;
	desc.DepthOrArraySize = 1;
	desc.Format           = format;
	desc.SampleDesc.Count = 1;
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

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
	DxObject::Assert(hr, L"depth stencil resource create failed.");

}

void DepthStencil::CreateDSV(Device* devices, DescriptorHeaps* descriptorHeaps, DXGI_FORMAT format) {

	// deviceの取得
	auto device = devices->GetDevice();

	// handleの取得
	descriptorDSV_ = descriptorHeaps->GetDescriptor(kDescriptor_DSV);

	// descの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
	desc.Format        = format;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	// DSVの生成
	device->CreateDepthStencilView(
		resource_.Get(),
		&desc,
		descriptorDSV_.GetCPUHandle()
	);
}
