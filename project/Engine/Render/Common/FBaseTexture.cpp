#include "FBaseTexture.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/System.h>

//* lib
#include <Lib/Geometry/Color4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Option structure methods
////////////////////////////////////////////////////////////////////////////////////////////

D3D12_RESOURCE_FLAGS FBaseTexture::Option::GetResourceFlags() const {
	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

	if (flag.Test(Flag::RenderTarget)) {
		flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	}

	if (flag.Test(Flag::UnorderedAccess)) {
		flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	return flags;
}

std::optional<D3D12_CLEAR_VALUE> FBaseTexture::Option::GetClearValue() const {
	if (!flag.Test(Flag::RenderTarget)) {
		return std::nullopt;
	}

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format   = format;
	clearValue.Color[0] = clearColor.r;
	clearValue.Color[1] = clearColor.g;
	clearValue.Color[2] = clearColor.b;
	clearValue.Color[3] = clearColor.a;

	return clearValue;
}

////////////////////////////////////////////////////////////////////////////////////////////
// FBaseTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FBaseTexture::Create(const Option& option) {

	// deviceの取り出し
	auto device = System::GetDxDevice()->GetDevice();

	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = option.size.x;
		desc.Height           = option.size.y;
		desc.MipLevels        = 1;
		desc.DepthOrArraySize = 1;
		desc.Format           = option.format;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = option.GetResourceFlags();

		std::optional<D3D12_CLEAR_VALUE> clearValue = option.GetClearValue();

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			kDefaultState_,
			clearValue.has_value() ? &clearValue.value() : nullptr,
			IID_PPV_ARGS(&resource_)
		);
		DxObject::Assert(hr, L"texture create failed.");

		resource_->SetName(L"FTexture");
	}

	if (option.flag.Test(Flag::RenderTarget)) { //!< RTVの生成

		// handleの取得
		descriptorRTV_ = System::GetDescriptor(kDescriptor_RTV);

		// descの設定
		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format        = option.format;
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		// RTVの生成
		device->CreateRenderTargetView(
			resource_.Get(),
			&desc,
			descriptorRTV_.GetCPUHandle()
		);
	}

	if (option.flag.Test(Flag::UnorderedAccess)) { //!< UAVの生成

		// handleの取得
		descriptorUAV_ = System::GetDescriptor(kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format        = option.format;
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
		descriptorSRV_ = System::GetDescriptor(kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                    = option.format;
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

	// 引数の保存
	option_ = option;

}

void FBaseTexture::Term() {
	descriptorSRV_.Delete();
	descriptorRTV_.Delete();
	descriptorUAV_.Delete();
	resource_.Reset();
}

D3D12_RESOURCE_BARRIER FBaseTexture::TransitionBeginRenderTarget() const {
	StreamLogger::AssertA(option_.flag.Test(Flag::RenderTarget), "Texture is not render target.");

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
	context->GetCommandList()->ClearRenderTargetView(
		descriptorRTV_.GetCPUHandle(),
		&option_.clearColor.r,
		0, nullptr
	);
}

D3D12_RESOURCE_BARRIER FBaseTexture::TransitionBeginUnordered() const {
	StreamLogger::AssertA(option_.flag.Test(Flag::UnorderedAccess), "Texture is not unordered access.");

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

const D3D12_CPU_DESCRIPTOR_HANDLE& FBaseTexture::GetCPUHandleRTV() const {
	StreamLogger::AssertA(option_.flag.Test(Flag::RenderTarget), "Texture is not render target.");
	return descriptorRTV_.GetCPUHandle();
}

const D3D12_GPU_DESCRIPTOR_HANDLE& FBaseTexture::GetGPUHandleUAV() const {
	StreamLogger::AssertA(option_.flag.Test(Flag::UnorderedAccess), "Texture is not unordered access.");
	return descriptorUAV_.GetGPUHandle();
}
