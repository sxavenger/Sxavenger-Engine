#include "UnorderedTexture.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UnorderedTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UnorderedTexture::Create(const Vector2ui& size, DXGI_FORMAT format) {

	// 引数の保存
	format_ = format;
	size_   = size;

	// deviceの取り出し
	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	// resourceの生成
	{
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = size.x;
		desc.Height           = size.y;
		desc.DepthOrArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.MipLevels        = 1;
		desc.Format           = format;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		

		device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&resource_)
		);

		resource_->SetName(L"unordered texture");
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

	// UAV - unorderedAccessViewの生成
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.ViewDimension      = D3D12_UAV_DIMENSION_TEXTURE2D;
		desc.Format             = format;

		// UAVDescriptorの取得
		descriptorUAV_ = SxavengerSystem::GetDescriptor(kDescriptor_UAV);
		
		device->CreateUnorderedAccessView(
			resource_.Get(),
			nullptr,
			&desc,
			descriptorUAV_.GetCPUHandle()
		);
	}
}

void UnorderedTexture::Term() {
	descriptorUAV_.Delete();
}

void UnorderedTexture::TransitionBeginUnordered(const DirectXQueueContext* context) {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.pResource   = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void UnorderedTexture::TransitionEndUnordered(const DirectXQueueContext* context) {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.pResource   = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void UnorderedTexture::Barrier(const DirectXQueueContext* context) {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource          = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}
