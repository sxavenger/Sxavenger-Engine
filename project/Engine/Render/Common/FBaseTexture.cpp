#include "FBaseTexture.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//* lib
#include <Lib/Geometry/Color4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FBaseTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FBaseTexture::Create(const Vector2ui& size, DXGI_FORMAT format) {

	// deviceの取り出し
	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	// 引数の保存
	format_ = format;

	{ //!< resourceの生成

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
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		// clearValueの設定
		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = format;

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
		desc.Format        = format;
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
		desc.Format        = format;
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
		desc.Format                    = format;
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

void FBaseTexture::Term() {
	resource_.Reset();
	descriptorSRV_.Delete();
	descriptorRTV_.Delete();
	descriptorUAV_.Delete();
}

D3D12_RESOURCE_BARRIER FBaseTexture::TransitionBeginRenderTarget() const {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = kDefaultState_;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.pResource   = GetResource();

	return barrier;
}

void FBaseTexture::TransitionBeginRenderTarget(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionBeginRenderTarget();
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

D3D12_RESOURCE_BARRIER FBaseTexture::TransitionEndRenderTarget() const {

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter  = kDefaultState_;
	barrier.Transition.pResource   = GetResource();

	return barrier;
}

void FBaseTexture::TransitionEndRenderTarget(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionEndRenderTarget();
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FBaseTexture::ClearRenderTarget(const DirectXQueueContext* context) const {
	static constexpr Color4f clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };

	context->GetCommandList()->ClearRenderTargetView(
		descriptorRTV_.GetCPUHandle(),
		&clearColor.r,
		0, nullptr
	);
}

D3D12_RESOURCE_BARRIER FBaseTexture::TransitionBeginUnordered() const {

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = kDefaultState_;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.pResource   = GetResource();

	return barrier;
}

void FBaseTexture::TransitionBeginUnordered(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionBeginUnordered();
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

D3D12_RESOURCE_BARRIER FBaseTexture::TransitionEndUnordered() const {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter  = kDefaultState_;
	barrier.Transition.pResource   = GetResource();

	return barrier;
}

void FBaseTexture::TransitionEndUnordered(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionEndUnordered();
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

D3D12_RESOURCE_BARRIER FBaseTexture::TransitionBeginState(D3D12_RESOURCE_STATES state) const {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = kDefaultState_;
	barrier.Transition.StateAfter  = state;
	barrier.Transition.pResource   = GetResource();

	return barrier;
}

void FBaseTexture::TransitionBeginState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionBeginState(state);
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

D3D12_RESOURCE_BARRIER FBaseTexture::TransitionEndState(D3D12_RESOURCE_STATES state) const {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = state;
	barrier.Transition.StateAfter  = kDefaultState_;
	barrier.Transition.pResource   = GetResource();

	return barrier;
}

void FBaseTexture::TransitionEndState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const {
	D3D12_RESOURCE_BARRIER barrier = TransitionEndState(state);
	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FBaseTexture::BarrierUAV(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource          = GetResource();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}
