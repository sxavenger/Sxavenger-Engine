#include "DxrMethod.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>
#include <MyEngine.h> // dxCommonが作られている前提

////////////////////////////////////////////////////////////////////////////////////////////
// namespace DxrMethod methods
////////////////////////////////////////////////////////////////////////////////////////////

ComPtr<ID3D12Resource> DxrMethod::CreateBuffer(
	D3D12_HEAP_TYPE heapType, size_t bufferSize, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initState) {
	
	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES prop = {};
	if (heapType == D3D12_HEAP_TYPE_DEFAULT) {
		prop = kDefaultHeapProp;

	} else if (heapType == D3D12_HEAP_TYPE_UPLOAD) {
		prop = kUploadHeapProp;
	}

	// resourceの生成
	ComPtr<ID3D12Resource> result;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width            = bufferSize;
	desc.Height           = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels        = 1;
	desc.Format           = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags            = flags;

	auto device = MyEngine::GetDevice();

	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		initState,
		nullptr,
		IID_PPV_ARGS(&result)
	);

	assert(SUCCEEDED(hr));

	return result;
}

void DxrMethod::ReleaseAndCreateBuffer(
	D3D12_HEAP_TYPE heapType,
	ComPtr<ID3D12Resource>& resource,
	size_t bufferSize, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initState) {

	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES prop = {};
	if (heapType == D3D12_HEAP_TYPE_DEFAULT) {
		prop = kDefaultHeapProp;

	} else if (heapType == D3D12_HEAP_TYPE_UPLOAD) {
		prop = kUploadHeapProp;
	}

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width            = bufferSize;
	desc.Height           = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels        = 1;
	desc.Format           = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags            = flags;

	auto device = MyEngine::GetDevice();

	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		initState,
		nullptr,
		IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
	);

	assert(SUCCEEDED(hr));

	return;
}
