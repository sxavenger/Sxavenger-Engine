#include "DepthBufferController.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DepthBufferController class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DepthBufferController::Create(const Vector2ui& size) {
	resterizerDepth_ = std::make_unique<DepthTexture>();
	resterizerDepth_->Create(size);

	CreateRaytracingDepth(size);
}

void DepthBufferController::Term() {
}

void DepthBufferController::TransitionBeginRaytracingDepthWrite(const DirectXThreadContext* context) {

	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.pResource   = raytracingDepthBuffer_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void DepthBufferController::TransitionEndRaytracingDepthWrite(const DirectXThreadContext* context) {

	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.pResource   = raytracingDepthBuffer_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void DepthBufferController::TransferRaytracingToRasterize(const DirectXThreadContext* context) {

	// commandListの取得
	auto commandList = context->GetCommandList();

	// copyするため, 各Resourceのstateの変更
	D3D12_RESOURCE_BARRIER barriers[2] = {};

	//* dst
	barriers[0].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	barriers[0].Transition.pResource   = resterizerDepth_->GetResource();
	barriers[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_DEST;

	//* src
	barriers[1].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	barriers[1].Transition.pResource   = raytracingDepthBuffer_.Get();
	barriers[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE;

	commandList->ResourceBarrier(_countof(barriers), barriers);

	// copy resion
	D3D12_TEXTURE_COPY_LOCATION src = {};
	src.pResource        = raytracingDepthBuffer_.Get();
	src.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

	D3D12_TEXTURE_COPY_LOCATION dst = {};
	dst.pResource        = resterizerDepth_->GetResource();
	dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

	commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	// 元のstateの状態に戻す
	//* dst
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barriers[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	//* src
	barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
	barriers[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	commandList->ResourceBarrier(_countof(barriers), barriers);
}

void DepthBufferController::TransitionBeginRasterizeDepthWrite(const DirectXThreadContext* context) {
	resterizerDepth_->TransitionBeginDepthWrite(context);
}

void DepthBufferController::TransitionEndRasterizeDepthWrite(const DirectXThreadContext* context) {
	resterizerDepth_->TransitionEndDepthWrite(context);
}

void DepthBufferController::ClearRasterizeDepth(const DirectXThreadContext* context) {
	resterizerDepth_->ClearDepth(context);
}

void DepthBufferController::CreateRaytracingDepth(const Vector2ui& size) {

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
		desc.Format           = DXGI_FORMAT_R32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&raytracingDepthBuffer_)
		);

		Assert(SUCCEEDED(hr));
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
			raytracingDepthBuffer_.Get(),
			nullptr,
			&desc,
			descriptorUAV_.GetCPUHandle()
		);

	}

}


