#include "DepthBufferController.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DepthBufferController class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DepthBufferContoller::Create(const Vector2ui& size) {

	// 引数の保存
	descriptorHeaps_ = Sxavenger::GetDxCommon()->GetDescriptorsObj();
	size_            = size;

	CreateRasterizeDepthBuffer();
	CreateRaytracingDepthBuffer();
}

void DepthBufferContoller::Term() {
}

void DepthBufferContoller::BeginRaytracingDepthWrite() {

	// commandListの取得
	auto commandList = Sxavenger::GetCommandList();

	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = raytracingDepthBuffer_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	commandList->ResourceBarrier(1, &barrier);
}

void DepthBufferContoller::EndRaytracingDepthWrite() {

	// commandListの取得
	auto commandList = Sxavenger::GetCommandList();

	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = raytracingDepthBuffer_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	commandList->ResourceBarrier(1, &barrier);

}

void DepthBufferContoller::TransferRaytracingToRasterize() {

	// commandListの取得
	auto commandList = Sxavenger::GetCommandList();

	// copyするため, 各Resourceのstateの変更
	D3D12_RESOURCE_BARRIER barriers[2] = {};

	//* dst
	barriers[0].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	barriers[0].Transition.pResource   = rasterizeDepthBuffer_.Get();
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
	dst.pResource        = rasterizeDepthBuffer_.Get();
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

void DepthBufferContoller::BeginRasterizeDepthWrite(bool isClearDepth) {

	// commandListの取得
	auto commandList = Sxavenger::GetCommandList();

	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = rasterizeDepthBuffer_.Get();
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

void DepthBufferContoller::EndRasterizeDepthWrite() {

	// commandListの取得
	auto commandList = Sxavenger::GetCommandList();

	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = rasterizeDepthBuffer_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	commandList->ResourceBarrier(1, &barrier);

}

void DepthBufferContoller::CreateRasterizeDepthBuffer() {

	// deviceの取得
	auto device = Sxavenger::GetDevice();

	{ //!< resourceの生成

		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = size_.x;
		desc.Height           = size_.y;
		desc.MipLevels        = 1;
		desc.DepthOrArraySize = 1;
		desc.Format           = DXGI_FORMAT_D32_FLOAT_S8X24_UINT; //!< R32_FLOATからcopy対象なるのでこのformatを使用
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
			IID_PPV_ARGS(&rasterizeDepthBuffer_)
		);

		Assert(SUCCEEDED(hr));
	}
	
	{ //!< DSVの生成

		// handleの取得
		descriptorDSV_ = descriptorHeaps_->GetDescriptor(DescriptorType::DSV);

		// descの設定
		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format        = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

		// DSVの生成
		device->CreateDepthStencilView(
			rasterizeDepthBuffer_.Get(),
			&desc,
			descriptorDSV_.GetCPUHandle()
		);
	}

	{ //!< SRVの生成

		// handleの取得
		descriptorSRV_ = descriptorHeaps_->GetDescriptor(DescriptorType::SRV);

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Texture2D.MipLevels     = 1;

		// SRVの生成
		device->CreateShaderResourceView(
			rasterizeDepthBuffer_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

}

void DepthBufferContoller::CreateRaytracingDepthBuffer() {

	// deviceの取得
	auto device = Sxavenger::GetDevice();

	{ //!< resourceの生成

		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = size_.x;
		desc.Height           = size_.y;
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
		descriptorUAV_ = descriptorHeaps_->GetDescriptor(UAV);

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
