#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxrObjectCommon
#include <DxrObjectCommon.h>

//* DxrObject
#include <DxrAccelerationStructure.h>
#include <DxrStateObject.h>
#include <DxrRootSignature.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderTable class
////////////////////////////////////////////////////////////////////////////////////////////
class ShaderTable {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ShaderTable() = default;

	void Create(
		const StateObject* stateObject, const TopLevelAS* tlas,
		const BufferRecord* raygenerationRecorder, const BufferRecord* missRecorder
	);

	const D3D12_DISPATCH_RAYS_DESC* GetDispatchRayDesc() const { return &dispathRayDesc_; }

private:

	//=========================================================================================
	// private varaibles
	//=========================================================================================

	//* ID3D12 *//

	ComPtr<ID3D12Resource> table_;

	//* DESC *//

	D3D12_DISPATCH_RAYS_DESC dispathRayDesc_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* write options *//

	static UINT Alignment(size_t size, UINT align);

	static UINT WirteShaderIdentifier(void* dst, const void* shaderId);
	static UINT WriteGPUAddress(void* dst, const D3D12_GPU_VIRTUAL_ADDRESS address);
	static UINT WriteGPUHandle(void* dst, const D3D12_GPU_DESCRIPTOR_HANDLE handle);

	static uint8_t* WriteShaderRecord(
		uint8_t* dst,
		const BufferRecord* recorder, UINT recordSize,
		ID3D12StateObjectProperties* prop);
};

////////////////////////////////////////////////////////////////////////////////////////////
// constexpr
////////////////////////////////////////////////////////////////////////////////////////////

constexpr const UINT kDefaultShaderRecordSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
constexpr const UINT kShaderRecordAlignment   = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;
constexpr const UINT kShaderTableAlignment    = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;

_DXROBJECT_NAMESPACE_END