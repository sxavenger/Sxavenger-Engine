#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include <Engine/System/DxrObject/DxrObjectCommon.h>
#include <Engine/System/DxrObject/DxrStateObject.h>
#include <Engine/System/DxrObject/DxrAccelerationStructure.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
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

	ShaderTable()  = default;
	~ShaderTable() = default;

	void Create(
		StateObject* stateObject, TopLevelAS* tlas,
		const BufferRecoreder* raygeneration, const BufferRecoreder* miss
	);

	void DispatchRays();

private:

	//=========================================================================================
	// private varaibles
	//=========================================================================================

	//* external *//

	ID3D12StateObjectProperties* properties_ = nullptr;

	//* member *//

	ComPtr<ID3D12Resource> table_;
	D3D12_DISPATCH_RAYS_DESC dispathRayDesc_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* writing methods *//

	uint8_t* WriteShaderRecord(uint8_t* dst, UINT recordSize, const BufferRecoreder* recorder);

	static UINT Alignment(size_t size, UINT align);
	static UINT WirteShaderIdentifier(void* dst, const void* shaderId);
	static UINT WriteGPUAddress(void* dst, const D3D12_GPU_VIRTUAL_ADDRESS& address);
	static UINT WriteGPUHandle(void* dst, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);
};

////////////////////////////////////////////////////////////////////////////////////////////
// constexpr
////////////////////////////////////////////////////////////////////////////////////////////

constexpr const UINT kDefaultShaderRecordSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
constexpr const UINT kShaderRecordAlignment   = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;
constexpr const UINT kShaderTableAlignment    = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;

_DXROBJECT_NAMESPACE_END