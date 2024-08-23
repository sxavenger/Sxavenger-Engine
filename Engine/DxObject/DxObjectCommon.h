#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DirectX12
#include <d3d12.h>
#include <dxgi1_6.h>

//* c++
#include <cstdint>
#include <cassert>
#include <optional>

//* ComPtr
#include <ComPtr.h>

//* Log
#include <Logger.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject Defines
////////////////////////////////////////////////////////////////////////////////////////////

//* DxObject namespace
#define _DXOBJECT DxObject::
#define _DXOBJECT_NAMESPACE_BEGIN namespace DxObject {
#define _DXOBJECT_NAMESPACE_END   }
#define _DXOBJECT_USING           using namespace DxObject;


////////////////////////////////////////////////////////////////////////////////////////////
// DxObject Commons
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderVisibility enum
////////////////////////////////////////////////////////////////////////////////////////////
enum ShaderVisibility {
	VISIBILITY_ALL           = D3D12_SHADER_VISIBILITY_ALL,
	VISIBILITY_VERTEX        = D3D12_SHADER_VISIBILITY_VERTEX,
	VISIBILITY_HULL          = D3D12_SHADER_VISIBILITY_HULL,
	VISIBILITY_DOMAIN        = D3D12_SHADER_VISIBILITY_DOMAIN,
	VISIBILITY_GEOMETRY      = D3D12_SHADER_VISIBILITY_GEOMETRY,
	VISIBILITY_PIXEL         = D3D12_SHADER_VISIBILITY_PIXEL,
	VISIBILITY_AMPLIFICATION = D3D12_SHADER_VISIBILITY_AMPLIFICATION,
	VISIBILITY_MESH          = D3D12_SHADER_VISIBILITY_MESH
};

////////////////////////////////////////////////////////////////////////////////////////////
// SamplerMode enum
////////////////////////////////////////////////////////////////////////////////////////////
enum SamplerMode {
	MODE_WRAP        = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	MODE_MIRROR      = D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
	MODE_CLAMP       = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	MODE_BORDER      = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
	MODE_MIRROR_ONCE = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE
};

////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum DescriptorType { //!< DxDescriptorHeapに合わせる
	RTV,
	DSV,
	CBV_SRV_UAV,

	kDescriptorHeapCount,

	SRV = CBV_SRV_UAV,
	UAV = CBV_SRV_UAV,
};

////////////////////////////////////////////////////////////////////////////////////////////
// Descriptor structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Descriptor {

	//* members *//

	DescriptorType type = kDescriptorHeapCount; //!< 無効値の場合"kDescriptorHeapCount"を入れる
	uint32_t       index;
	
	//! [pair]
	//! first:  CPUHandle
	//! second: GPUHandle(取得するDescriptorHeapによって無効値がある)
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, std::optional<D3D12_GPU_DESCRIPTOR_HANDLE>> handles;

	//* methods *//

	void Term() {
		type = kDescriptorHeapCount; //!< 無効値の代入
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle() const {
		return handles.first;
	}

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const {
		assert(handles.second); //!< GPUDescriptorを持っていないので
		return handles.second.value();
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

ComPtr<ID3D12Resource> CreateBufferResource(
	ID3D12Device* device,
	D3D12_HEAP_TYPE heapType,
	size_t sizeInBytes,
	D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state
);

ComPtr<ID3D12Resource> CreateBufferResource(
	ID3D12Device* device,
	size_t sizeInBytes
);

UINT RoundUp(UINT round, UINT thread);

////////////////////////////////////////////////////////////////////////////////////////////
// Configs
////////////////////////////////////////////////////////////////////////////////////////////

//!< 最終的な画面に映るformat
static const DXGI_FORMAT forwardFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

_DXOBJECT_NAMESPACE_END