#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>

// c++
#include <cstdint>
#include <vector>

// ComPtr
#include <ComPtr.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// namespace DxrMethod
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxrMethod {

	ComPtr<ID3D12Resource> CreateBuffer(
		D3D12_HEAP_TYPE heapType, size_t size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initState
	);

	// ヒーププロパティ
	static const D3D12_HEAP_PROPERTIES kDefaultHeapProp{
		D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
	};

	static const D3D12_HEAP_PROPERTIES kUploadHeapProp{
		D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
	};

}