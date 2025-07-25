#include "RenderTexture.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//=========================================================================================
// static const variables
//=========================================================================================

const Color4f RenderTexture::kDefaultClearColor = Color4f::Convert(0x3A504BFF);

////////////////////////////////////////////////////////////////////////////////////////////
// RenderTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderTexture::Create(const Vector2ui& size, const Color4f& clearColor, DXGI_FORMAT format) {

	clearColor_ = clearColor;
	format_     = format;
	size_       = size;

	// deviceの取り出し
	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	// resourceの生成
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = size.x;
		desc.Height           = size.y;
		desc.MipLevels        = 1;
		desc.Format           = format;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		desc.SampleDesc.Count = 1;
		desc.DepthOrArraySize = 1;

		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format   = format;
		clearValue.Color[0] = clearColor_.r;
		clearValue.Color[1] = clearColor_.g;
		clearValue.Color[2] = clearColor_.b;
		clearValue.Color[3] = clearColor_.a;

		device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(&resource_)
		);

		resource_->SetName(L"render texture");
	}

	// SRV - shaderResourceViewの生成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = format;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels     = 1;

		// SRVを生成するDescriptorHeapの場所を決める
		descriptorSRV_ = SxavengerSystem::GetDescriptor(kDescriptor_SRV);

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

	// RTV - RenderTargetViewの生成
	{
		descriptorRTV_ = SxavengerSystem::GetDescriptor(kDescriptor_RTV);

		device->CreateRenderTargetView(
			resource_.Get(),
			nullptr,
			descriptorRTV_.GetCPUHandle()
		);
	}
}

void RenderTexture::Term() {
	descriptorRTV_.Delete();
}

void RenderTexture::TransitionBeginRenderTarget(const DirectXQueueContext* context) {

	auto commandList = context->GetCommandList();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.pResource   = resource_.Get();

	commandList->ResourceBarrier(1, &barrier);
}

void RenderTexture::TransitionEndRenderTarget(const DirectXQueueContext* context) {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.pResource   = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void RenderTexture::ClearRenderTarget(const DirectXQueueContext* context) {
	// 画面のクリア
	context->GetCommandList()->ClearRenderTargetView(
		descriptorRTV_.GetCPUHandle(),
		&clearColor_.r,
		0, nullptr
	);
}
