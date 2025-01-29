#include "FSceneDepth.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneDepth RasterizerDepth structure method
////////////////////////////////////////////////////////////////////////////////////////////

void FSceneDepth::RasterizerDepth::Create(const Vector2ui& size) {

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
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clear,
			IID_PPV_ARGS(&resource_)
		);

		Assert(SUCCEEDED(hr));

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
		desc.Format                  = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;  //!< DXGI_FORMAT_D32_FLOAT_S8X24_UINT限定
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

void FSceneDepth::RasterizerDepth::Delete() {
	resource_.Reset();
	descriptorDSV_.Delete();
	descriptorSRV_.Delete();
}

void FSceneDepth::RasterizerDepth::BeginWrite(const DirectXThreadContext* context) const {
	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_DEPTH_WRITE;

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FSceneDepth::RasterizerDepth::EndWrite(const DirectXThreadContext* context) const {
	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FSceneDepth::RasterizerDepth::ClearDepth(const DirectXThreadContext* context) const {
	// 深度をクリア
	context->GetCommandList()->ClearDepthStencilView(
		descriptorDSV_.GetCPUHandle(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0, 0, nullptr
	);
}

void FSceneDepth::RasterizerDepth::BeginState(const DirectXThreadContext* context, D3D12_RESOURCE_STATES state) const {
	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = state;

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FSceneDepth::RasterizerDepth::EndState(const DirectXThreadContext* context, D3D12_RESOURCE_STATES state) const {
	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resource_.Get();
	barrier.Transition.StateBefore = state;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneDepth RaytracingDepth structure method
////////////////////////////////////////////////////////////////////////////////////////////

void FSceneDepth::RaytracingDepth::Create(const Vector2ui& size) {

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
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&resource_)
		);

		Assert(SUCCEEDED(hr));

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

void FSceneDepth::RaytracingDepth::Delete() {
	resource_.Reset();
	descriptorUAV_.Delete();
}

void FSceneDepth::RaytracingDepth::BeginWrite(const DirectXThreadContext* context) const {
	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.pResource   = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FSceneDepth::RaytracingDepth::EndWrite(const DirectXThreadContext* context) const {
	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.pResource   = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Framework SceneDepth class method
////////////////////////////////////////////////////////////////////////////////////////////

void FSceneDepth::Create(const Vector2ui& size) {
	rasterizer_.Create(size);
	raytracing_.Create(size);
}

void FSceneDepth::Term() {
	rasterizer_.Delete();
	raytracing_.Delete();
}

void FSceneDepth::TransitionBeginRasterizer(const DirectXThreadContext* context) const {
	rasterizer_.BeginWrite(context);
}

void FSceneDepth::TransitionEndRasterizer(const DirectXThreadContext* context) const {
	rasterizer_.EndWrite(context);
}

void FSceneDepth::ClearRasterizerDepth(const DirectXThreadContext* context) const {
	rasterizer_.ClearDepth(context);
}

void FSceneDepth::TransitionBeginRaytracing(const DirectXThreadContext* context) const {
	raytracing_.BeginWrite(context);
}

void FSceneDepth::TransitionEndRaytracing(const DirectXThreadContext* context) const {
	raytracing_.EndWrite(context);
}

void FSceneDepth::TransitionBeginStateRasterizer(const DirectXThreadContext* context, D3D12_RESOURCE_STATES state) const {
	rasterizer_.BeginState(context, state);
}

void FSceneDepth::TransitionEndStateRasterizer(const DirectXThreadContext* context, D3D12_RESOURCE_STATES state) const {
	rasterizer_.EndState(context, state);
}
