#include "DxCSBufferResource.h"

//-----------------------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////
// CSBaseBufferResource class
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::CSBaseBufferResource::Init(Devices* devices, uint32_t indexSize, size_t structureSize) {
	// 引数の確認
	assert(structureSize != 0 || indexSize != 0);

	// sizeの保存
	indexSize_     = indexSize;
	structureSize_ = static_cast<UINT>(structureSize);

	// deviceの取り出し
	auto device = devices->GetDevice();

	// resourceの生成
	uavResource_ = DxMethods::CreateResource(
		device,
		D3D12_HEAP_TYPE_DEFAULT,
		indexSize_ * structureSize_,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON
	);

	readBackResource_ = DxMethods::CreateResource(
		device,
		D3D12_HEAP_TYPE_READBACK,
		indexSize_ * structureSize_,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	
	//!< mapは各派生クラスで行う
}

void DxObject::CSBaseBufferResource::Term() {
}

void DxObject::CSBaseBufferResource::CopyReadBack(ID3D12GraphicsCommandList* commandList) {

	// uavResourceをコピー元になるよう設定
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource   = uavResource_.Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE;

		// バリアの実行
		commandList->ResourceBarrier(1, &barrier);
	}

	// copyの実行
	commandList->CopyResource(readBackResource_.Get(), uavResource_.Get());

	// uavResourceを元に戻す
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource   = uavResource_.Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

		// バリアの実行
		commandList->ResourceBarrier(1, &barrier);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// DxMethods namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

ComPtr<ID3D12Resource> DxMethods::CreateResource(
	ID3D12Device* device, D3D12_HEAP_TYPE heapType, size_t sizeInBytes, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state) {

	ComPtr<ID3D12Resource> result;

	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = heapType;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width            = sizeInBytes;
	desc.Height           = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels        = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags            = flags;

	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		state,
		nullptr,
		IID_PPV_ARGS(&result)
	);

	assert(SUCCEEDED(hr));

	return result;
}
