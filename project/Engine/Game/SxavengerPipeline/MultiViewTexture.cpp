#include "MultiViewTexture.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MultiViewTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MultiViewTexture::Create(
	MultiViewFlag flag, const Vector2ui& size, const Color4f& clearColor, DXGI_FORMAT format) {

	// 引数の保存
	size_       = size;
	format_     = format;
	clearColor_ = clearColor;

	// externalの保存
	descriptorHeaps_ = Sxavenger::GetDxCommon()->GetDescriptorsObj();

	CreateResource();

	if (flag & VIEWFLAG_SRV) {
		CreateSRV();
	}

	if (flag & VIEWFLAG_RTV) {
		CreateRTV();
	}

	if (flag & VIEWFLAG_UAV) {
		CreateUAV();
	}
}

void MultiViewTexture::Term() {
	// descriptorの返却
	for (uint32_t i = 0; i < MultiViewType::kCountOfMultiViewType; ++i) {
		descriptorHeaps_->DeleteDescriptor(descriptors_[i]);
	}

	descriptorHeaps_ = nullptr;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& MultiViewTexture::GetGPUHandleSRV() const {
	Assert(IsCreateSRV(), "Not create VIEWFLAG_SRV."); //!< SRVが作成されていない
	return descriptors_[VIEWTYPE_SRV].GetGPUHandle();
}

const D3D12_CPU_DESCRIPTOR_HANDLE& MultiViewTexture::GetCPUHandleRTV() const {
	Assert(IsCreateRTV(), "Not create VIEWFLAG_RTV."); //!< RTVが生成されていない
	return descriptors_[VIEWTYPE_RTV].GetCPUHandle();
}

const D3D12_GPU_DESCRIPTOR_HANDLE& MultiViewTexture::GetGPUHandleUAV() const {
	Assert(IsCreateUAV(), "Not create VIEWFLAG_UAV."); //!< UAVが生成されていない
	return descriptors_[VIEWTYPE_UAV].GetGPUHandle();
}

void MultiViewTexture::CreateResource() {

	// propの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	// descの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width            = size_.x;
	desc.Height           = size_.y;
	desc.MipLevels        = 1;
	desc.Format           = format_;
	desc.SampleDesc.Count = 1;
	desc.DepthOrArraySize = 1;
	desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	// clearColorの設定
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format   = format_;
	clearValue.Color[0] = clearColor_.r;
	clearValue.Color[1] = clearColor_.g;
	clearValue.Color[2] = clearColor_.b;
	clearValue.Color[3] = clearColor_.a;

	// deviceの取得
	auto device = Sxavenger::GetDevice();

	// resourceの生成
	device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
		&desc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(&resource_)
	);
}

void MultiViewTexture::CreateSRV() {
	if (viewRecorder_ & VIEWFLAG_SRV) {
		return; //!< すでに作られている
	}

	// desc設定
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format                  = format_;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels     = 1;

	// descriptorの取得
	descriptors_[VIEWTYPE_SRV] = descriptorHeaps_->GetDescriptor(SRV);

	// deviceの取得
	auto device = Sxavenger::GetDevice();

	// SRVの生成
	device->CreateShaderResourceView(
		resource_.Get(),
		&desc,
		descriptors_[VIEWTYPE_SRV].GetCPUHandle()
	);

	// recordに記録
	viewRecorder_ |= VIEWFLAG_SRV;
}

void MultiViewTexture::CreateRTV() {
	if (viewRecorder_ & VIEWFLAG_RTV) {
		return; //!< すでに作られている
	}

	// descriptorの取得
	descriptors_[VIEWTYPE_RTV] = descriptorHeaps_->GetDescriptor(RTV);

	// deviceの取得
	auto device = Sxavenger::GetDevice();

	// RTVの生成
	device->CreateRenderTargetView(
		resource_.Get(),
		nullptr,
		descriptors_[VIEWTYPE_RTV].GetCPUHandle()
	);

	// recordに記録
	viewRecorder_ |= VIEWFLAG_RTV;
}

void MultiViewTexture::CreateUAV() {
	if (viewRecorder_ & VIEWFLAG_UAV) {
		return; //!< すでに作られている
	}

	// desc設定
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.ViewDimension      = D3D12_UAV_DIMENSION_TEXTURE2D;
	desc.Format             = format_;

	// descriptorの取得
	descriptors_[VIEWTYPE_UAV] = descriptorHeaps_->GetDescriptor(UAV);

	// deviceの取得
	auto device = Sxavenger::GetDevice();

	// UAVの生成
	device->CreateUnorderedAccessView(
		resource_.Get(),
		nullptr,
		&desc,
		descriptors_[VIEWTYPE_UAV].GetCPUHandle()
	);

	// recordに記録
	viewRecorder_ |= VIEWFLAG_UAV;
}
