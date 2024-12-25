#include "MultiViewTextureBuffer.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MultiViewTextureBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MultiViewTextureBuffer::Create(
	uint8_t multiViewFlag, const Vector2ui& size, const Color4f& clearColor, DXGI_FORMAT format) {

	// 引数の保存
	size_ = size;
	format_ = format;
	clearColor_ = clearColor;

	CreateResource();

	if (multiViewFlag & VIEWFLAG_SRV) {
		CreateSRV();
		viewRecorder_.set(ViewType::kSRV);
	}

	if (multiViewFlag & VIEWFLAG_RTV) {
		CreateRTV();
		viewRecorder_.set(ViewType::kRTV);
	}

	if (multiViewFlag & VIEWFLAG_UAV) {
		CreateUAV();
		viewRecorder_.set(ViewType::kUAV);
	}
}

void MultiViewTextureBuffer::Term() {
	// descriptorの返却
	for (auto& descriptor : descriptors_) {
		descriptor.Delete();
	}
}

void MultiViewTextureBuffer::TransitionBeginRenderTarget(const DirectXThreadContext* context) {
	Assert(IsCreatedRTV(), "Not create RTV."); //!< RTVが生成されていない

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.pResource = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void MultiViewTextureBuffer::TransitionEndRenderTarget(const DirectXThreadContext* context) {
	Assert(IsCreatedRTV(), "Not create RTV."); //!< RTVが生成されていない

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.pResource = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void MultiViewTextureBuffer::ClearRenderTarget(const DirectXThreadContext* context) {
	Assert(IsCreatedRTV(), "Not create RTV."); //!< RTVが生成されていない

	// 画面のクリア
	context->GetCommandList()->ClearRenderTargetView(
		GetCPUHandleRTV(),
		&clearColor_.r,
		0, nullptr
	);
}

void MultiViewTextureBuffer::TransitionBeginUnordered(const DirectXThreadContext* context) {
	Assert(IsCreatedUAV(), "Not create UAV."); //!< RTVが生成されていない

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.pResource = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void MultiViewTextureBuffer::TransitionEndUnordered(const DirectXThreadContext* context) {
	Assert(IsCreatedUAV(), "Not create UAV."); //!< RTVが生成されていない

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.pResource = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

const D3D12_GPU_DESCRIPTOR_HANDLE& MultiViewTextureBuffer::GetGPUHandleSRV() const {
	Assert(IsCreatedSRV(), "Not create SRV."); //!< SRVが作成されていない
	return descriptors_[ViewType::kSRV].GetGPUHandle();
}

const D3D12_CPU_DESCRIPTOR_HANDLE& MultiViewTextureBuffer::GetCPUHandleRTV() const {
	Assert(IsCreatedRTV(), "Not create RTV."); //!< RTVが生成されていない
	return descriptors_[ViewType::kRTV].GetCPUHandle();
}

const D3D12_GPU_DESCRIPTOR_HANDLE& MultiViewTextureBuffer::GetGPUHandleUAV() const {
	Assert(IsCreatedUAV(), "Not create UAV."); //!< UAVが生成されていない
	return descriptors_[ViewType::kUAV].GetGPUHandle();
}

bool MultiViewTextureBuffer::IsCreatedSRV() const {
	return viewRecorder_.test(ViewType::kSRV);
}

bool MultiViewTextureBuffer::IsCreatedRTV() const {
	return viewRecorder_.test(ViewType::kRTV);
}

bool MultiViewTextureBuffer::IsCreatedUAV() const {
	return viewRecorder_.test(ViewType::kUAV);
}

void MultiViewTextureBuffer::CreateResource() {

	// propの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	// descの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = size_.x;
	desc.Height = size_.y;
	desc.MipLevels = 1;
	desc.Format = format_;
	desc.SampleDesc.Count = 1;
	desc.DepthOrArraySize = 1;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	// clearColorの設定
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = format_;
	clearValue.Color[0] = clearColor_.r;
	clearValue.Color[1] = clearColor_.g;
	clearValue.Color[2] = clearColor_.b;
	clearValue.Color[3] = clearColor_.a;

	// resourceの生成
	SxavengerSystem::GetDxDevice()->GetDevice()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
		&desc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(&resource_)
	);

}

void MultiViewTextureBuffer::CreateSRV() {

	// desc設定
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = format_;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = 1;

	// descriptorの取得
	descriptors_[ViewType::kSRV] = SxavengerSystem::GetDescriptor(kDescriptor_SRV);

	// SRVの生成
	SxavengerSystem::GetDxDevice()->GetDevice()->CreateShaderResourceView(
		resource_.Get(),
		&desc,
		descriptors_[ViewType::kSRV].GetCPUHandle()
	);
}

void MultiViewTextureBuffer::CreateRTV() {

	// descriptorの取得
	descriptors_[ViewType::kRTV] = SxavengerSystem::GetDescriptor(kDescriptor_RTV);

	// RTVの生成
	SxavengerSystem::GetDxDevice()->GetDevice()->CreateRenderTargetView(
		resource_.Get(),
		nullptr,
		descriptors_[ViewType::kRTV].GetCPUHandle()
	);
}

void MultiViewTextureBuffer::CreateUAV() {

	// desc設定
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	desc.Format = format_;

	// descriptorの取得
	descriptors_[ViewType::kUAV] = SxavengerSystem::GetDescriptor(kDescriptor_UAV);

	// UAVの生成
	SxavengerSystem::GetDxDevice()->GetDevice()->CreateUnorderedAccessView(
		resource_.Get(),
		nullptr,
		&desc,
		descriptors_[ViewType::kUAV].GetCPUHandle()
	);
}
