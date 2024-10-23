#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObjectCommon
#include <DxObjectCommon.h>

//* c++
#include <variant>

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject Defines
////////////////////////////////////////////////////////////////////////////////////////////

//* DxObject namespace
#define _DXROBJECT_NAMESPACE_BEGIN namespace DxrObject {
#define _DXROBJECT_NAMESPACE_END   }
#define _DXROBJECT                 DxrObject::
#define _DXROBJECT_USING           using namespace DxrObject;

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject commons
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using GPUAddress = std::optional<D3D12_GPU_VIRTUAL_ADDRESS>;
using GPUHandle  = std::optional<D3D12_GPU_DESCRIPTOR_HANDLE>;
using GPUBuffer  = std::variant<GPUAddress, GPUHandle>;

//-----------------------------------------------------------------------------------------
// constexpr
//-----------------------------------------------------------------------------------------
static_assert(
	sizeof(D3D12_GPU_VIRTUAL_ADDRESS) == sizeof(D3D12_GPU_DESCRIPTOR_HANDLE),
	"[RayTracing Error]: DxrObjectCommon Enviornment\n Cannot use [size_t kGPUBufferSize]. different sizeof: D3D12_GPU_VIRTUAL_ADDRESS, D3D12_GPU_DESCRIPTOR_HANDLE"
);
constexpr const size_t kGPUBufferSize = sizeof(D3D12_GPU_VIRTUAL_ADDRESS);
//!< static_assertで"D3D12_GPU_VIRTUAL_ADDRESS"と"D3D12_GPU_VIRTUAL_ADDRESS"のbufferSizeは同じであることを保証している


_DXROBJECT_NAMESPACE_END