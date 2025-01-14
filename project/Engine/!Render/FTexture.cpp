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
		desc.Width  = size_.x;
		desc.Height = size_.y;
		desc.MipLevels = 1;
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
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(&resource_)
		);
		Assert(SUCCEEDED(hr));


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
}

void FTexture::Term() {
	resource_.Reset();
	descriptorUAV_.Delete();
	descriptorRTV_.Delete();
}

void FTexture::TransitionBeginRenderTarget(const DirectXThreadContext* context) const {

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.pResource   = GetResource();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FTexture::TransitionEndRenderTarget(const DirectXThreadContext* context) const {

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.pResource   = GetResource();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FTexture::ClearRenderTarget(const DirectXThreadContext* context) const {

	static const Color4f clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };

	context->GetCommandList()->ClearRenderTargetView(
		descriptorRTV_.GetCPUHandle(),
		&clearColor.r,
		0, nullptr
	);
}

void FTexture::TransitionBeginUnordered(const DirectXThreadContext* context) const {

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.pResource   = GetResource();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FTexture::TransitionEndUnordered(const DirectXThreadContext* context) const {

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.pResource   = GetResource();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}
