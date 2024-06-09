#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxrObject
#include <DxrStateObject.h>
#include <DxrResultBuffer.h>
#include <DxrRecordBuffer.h>
#include <DxrMethod.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxrObject {

	//-----------------------------------------------------------------------------------------
	// forward
	//-----------------------------------------------------------------------------------------
	class TopLevelAS;
	class BottomLevelAS;
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// ShaderTable class
	////////////////////////////////////////////////////////////////////////////////////////////
	class ShaderTable {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		~ShaderTable() { Term(); }

		void Init(int32_t clientWidth, int32_t clientHeight);

		void Record(
			TopLevelAS* tlas, StateObject* stateObject,
			RecordBuffer* raygenearation, RecordBuffer* miss // HACK:
		);

		void Term();

		const D3D12_DISPATCH_RAYS_DESC* GetDispatchRayDesc() const { return &dispatchRayDesc_; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		ComPtr<ID3D12Resource> table_;

		D3D12_DISPATCH_RAYS_DESC dispatchRayDesc_;

		// parameter //

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// constexpr
	////////////////////////////////////////////////////////////////////////////////////////////

	constexpr const UINT kShaderRecordAlignment   = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;
	constexpr const UINT kDefaultShaderRecordSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	constexpr const UINT kTableAlignment          = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;

}

////////////////////////////////////////////////////////////////////////////////////////////
// DxrMethod namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxrMethod {

	UINT Alignment(size_t size, UINT align);

	UINT WriteShaderIdentifier(void* dst, const void* shaderId);

	UINT WriteGPUDescriptor(void* dst, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

	UINT WriteGPUVirtualAddress(void* dst, const D3D12_GPU_VIRTUAL_ADDRESS& address);

	uint8_t* WriteShaderRecord(
		uint8_t* dst,
		DxrObject::RecordBuffer* buffer, UINT recordSize,
		ID3D12StateObjectProperties* prop
	);

}
