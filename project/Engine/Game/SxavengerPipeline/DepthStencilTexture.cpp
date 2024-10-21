#include "DepthStencilTexture.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DepthStencilTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DepthStencilTexture::Create(const Vector2ui& size) {

	// externalの保存
	descriptorHeaps_ = Sxavenger::GetDxCommon()->GetDescriptorsObj();

	// 引数の保存
	size_ = size;

	CreateResource();
	CreateDSV();
	CreateSRV();
}

void DepthStencilTexture::Term() {
}

void DepthStencilTexture::BeginDepthWrite(bool isClearDepth) {

	// commandListの取得
	auto commandList = Sxavenger::GetCommandList();

	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_DEPTH_WRITE;

	commandList->ResourceBarrier(1, &barrier);

	if (!isClearDepth) {
		return; //!< depth clearしない
	}

	// 深度をクリア
	commandList->ClearDepthStencilView(
		descriptorDSV_.GetCPUHandle(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0, 0, nullptr
	);
}

void DepthStencilTexture::EndDepthWrite() {

	// commandListの取得
	auto commandList = Sxavenger::GetCommandList();

	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	commandList->ResourceBarrier(1, &barrier);
}

void DepthStencilTexture::CreateResource() {
	// deviceの取得
	auto device = Sxavenger::GetDevice();

	// propの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	// descの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Width            = size_.x;
	desc.Height           = size_.y;
	desc.MipLevels        = 1;
	desc.DepthOrArraySize = 1;
	desc.Format           = DXGI_FORMAT_D24_UNORM_S8_UINT;
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
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clear,
		IID_PPV_ARGS(&resource_)
	);

	Assert(SUCCEEDED(hr));
}

void DepthStencilTexture::CreateDSV() {

	// deviceの取得
	auto device = Sxavenger::GetDevice();

	// handleの取得
	descriptorDSV_ = descriptorHeaps_->GetDescriptor(DescriptorType::DSV);

	// descの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	// DSVの生成
	device->CreateDepthStencilView(
		resource_.Get(),
		&desc,
		descriptorDSV_.GetCPUHandle()
	);
}

void DepthStencilTexture::CreateSRV() {

	// deviceの取得
	auto device = Sxavenger::GetDevice();

	// handleの取得
	descriptorSRV_ = descriptorHeaps_->GetDescriptor(DescriptorType::SRV);

	// descの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format                  = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Texture2D.MipLevels     = 1;

	//SRVの生成
	device->CreateShaderResourceView(
		resource_.Get(),
		&desc,
		descriptorSRV_.GetCPUHandle()
	);

}
