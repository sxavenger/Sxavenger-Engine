#include "FDepthTexture.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRasterizerDepth structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void FDepthTexture::FRasterizerDepth::Create(const Vector2ui& size) {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

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
		desc.Format           = DxObject::kDefaultDepthFormat;
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
			kDefaultResourceState_,
			&clear,
			IID_PPV_ARGS(&resource_)
		);

		Exception::Assert(SUCCEEDED(hr));

		resource_->SetName(L"FSceneDepth Rasterizer");
	}

	{ //!< DSVの生成

		// handleの取得
		descriptorDSV_ = SxavengerSystem::GetDescriptor(kDescriptor_DSV);

		// descの設定
		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format        = DxObject::kDefaultDepthFormat;
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

		// DSVの生成
		device->CreateDepthStencilView(
			resource_.Get(),
			&desc,
			descriptorDSV_.GetCPUHandle()
		);
	}

	{ //!< SRVの生成

		// handleの取得
		descriptorSRV_ = SxavengerSystem::GetDescriptor(DxObject::kDescriptor_SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = DxObject::kDefaultDepthViewFormat;  //!< DXGI_FORMAT_D32_FLOAT_S8X24_UINT限定
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Texture2D.MipLevels     = 1;

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

}

void FDepthTexture::FRasterizerDepth::Delete() {
	resource_.Reset();
	descriptorDSV_.Delete();
	descriptorSRV_.Delete();
}

void FDepthTexture::FRasterizerDepth::BeginWrite(const DirectXQueueContext* context) const {
	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = kDefaultResourceState_;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_DEPTH_WRITE;

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FDepthTexture::FRasterizerDepth::EndWrite(const DirectXQueueContext* context) const {
	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	barrier.Transition.StateAfter  = kDefaultResourceState_;

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FDepthTexture::FRasterizerDepth::ClearDepth(const DirectXQueueContext* context) const {
	// 深度をクリア
	context->GetCommandList()->ClearDepthStencilView(
		descriptorDSV_.GetCPUHandle(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0, 0, nullptr
	);
}

void FDepthTexture::FRasterizerDepth::BeginState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const {
	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = kDefaultResourceState_;
	barrier.Transition.StateAfter  = state;

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FDepthTexture::FRasterizerDepth::EndState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const {
	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = state;
	barrier.Transition.StateAfter  = kDefaultResourceState_;

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

////////////////////////////////////////////////////////////////////////////////////////////
// FRaytracingDepth structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void FDepthTexture::FRaytracingDepth::Create(const Vector2ui& size) {

	// deviceの取得
	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	{ //!< resourceの生成

		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = size.x;
		desc.Height           = size.y;
		desc.MipLevels        = 1;
		desc.DepthOrArraySize = 1;
		desc.Format           = DXGI_FORMAT_R32_FLOAT; //!< DefaultDepthと互換性のあるformatを使用
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			kDefaultResourceState_,
			nullptr,
			IID_PPV_ARGS(&resource_)
		);

		Exception::Assert(SUCCEEDED(hr));

		resource_->SetName(L"FSceneDepth Raytracing");
	}

	{ //!< UAVの生成

		// handleの取得
		descriptorUAV_ = SxavengerSystem::GetDescriptor(kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format        = DXGI_FORMAT_R32_FLOAT;
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

void FDepthTexture::FRaytracingDepth::Delete() {
	resource_.Reset();
	descriptorUAV_.Delete();
}

void FDepthTexture::FRaytracingDepth::BeginWrite(const DirectXQueueContext* context) const {
	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = kDefaultResourceState_;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.pResource   = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FDepthTexture::FRaytracingDepth::EndWrite(const DirectXQueueContext* context) const {
	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter  = kDefaultResourceState_;
	barrier.Transition.pResource   = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FDepthTexture::FRaytracingDepth::BeginState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const {
	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = kDefaultResourceState_;
	barrier.Transition.StateAfter  = state;
	barrier.Transition.pResource   = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FDepthTexture::FRaytracingDepth::EndState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const {
	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = state;
	barrier.Transition.StateAfter  = kDefaultResourceState_;
	barrier.Transition.pResource   = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

////////////////////////////////////////////////////////////////////////////////////////////
// FDepthTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FDepthTexture::Create(const Vector2ui& size) {
	rasterizer_.Create(size);
	raytracing_.Create(size);
}

void FDepthTexture::Term() {
	rasterizer_.Delete();
	raytracing_.Delete();
}

void FDepthTexture::TransitionBeginRasterizer(const DirectXQueueContext* context) const {
	rasterizer_.BeginWrite(context);
}

void FDepthTexture::TransitionEndRasterizer(const DirectXQueueContext* context) const {
	rasterizer_.EndWrite(context);
}

void FDepthTexture::ClearRasterizerDepth(const DirectXQueueContext* context) const {
	rasterizer_.ClearDepth(context);
}

void FDepthTexture::TransitionBeginRaytracing(const DirectXQueueContext* context) const {
	raytracing_.BeginWrite(context);
}

void FDepthTexture::TransitionEndRaytracing(const DirectXQueueContext* context) const {
	raytracing_.EndWrite(context);
}

void FDepthTexture::TransitionBeginStateRasterizer(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const {
	rasterizer_.BeginState(context, state);
}

void FDepthTexture::TransitionEndStateRasterizer(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const {
	rasterizer_.EndState(context, state);
}


void FDepthTexture::CopyRaytracingToRasterizer(const DirectXQueueContext* context) const {
	raytracing_.BeginState(context, D3D12_RESOURCE_STATE_COPY_SOURCE);
	rasterizer_.BeginState(context, D3D12_RESOURCE_STATE_COPY_DEST);

	// HACK: copy region
	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource = raytracing_.resource_.Get();
	src.Type      = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource = rasterizer_.resource_.Get();
	dst.Type      = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

	context->GetCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	raytracing_.EndState(context, D3D12_RESOURCE_STATE_COPY_SOURCE);
	rasterizer_.EndState(context, D3D12_RESOURCE_STATE_COPY_DEST);
}
