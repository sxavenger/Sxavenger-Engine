#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include <Engine/System/DirectX/DxObject/DxObjectCommon.h>

//* c++
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT define
////////////////////////////////////////////////////////////////////////////////////////////

#define _DXROBJECT_NAMESPACE_BEGIN namespace DxrObject {
#define _DXROBJECT_NAMESPACE_END   }
#define _DXROBJECT                 DxrObject::
#define _DXROBJECT_USING           using namespace DxrObject;

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT commons
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

UINT Alignment(size_t size, UINT align);

////////////////////////////////////////////////////////////////////////////////////////////
// environment
////////////////////////////////////////////////////////////////////////////////////////////

static_assert(sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) == sizeof(D3D12_GPU_VIRTUAL_ADDRESS), "GPUBuffer stride is 8 bytes.");
constexpr const size_t kBufferStride = sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);

//* shader table *//
constexpr const UINT kShaderRecordSize      = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
constexpr const UINT kShaderRecordAlignment = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;
constexpr const UINT kShaderTableAlignment  = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;

_DXROBJECT_NAMESPACE_END
