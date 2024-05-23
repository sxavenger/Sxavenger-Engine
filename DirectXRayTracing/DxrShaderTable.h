#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxrObject
#include <DxrStateObject.h>
#include <DxrResultBuffer.h>
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

		void Init(
			int32_t clientWidth, int32_t clientHeight,
			const StateObjectDesc& descs,
			TopLevelAS* tlas, StateObject* stateObject, ResultBuffer* resultBuffer);

		void Term();

		const D3D12_DISPATCH_RAYS_DESC* GetDispatchRayDesc() const { return &dispatchRayDesc_; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		ComPtr<ID3D12Resource> table_;

		D3D12_DISPATCH_RAYS_DESC dispatchRayDesc_;

		// record parameter //

		static const UINT   kShaderRecordAlignment_;
		static const UINT   kDefaultShaderRecordSize_;
		static const size_t kDescriptorGPUHandleSize_;
		static const UINT   kTableAlignment_;

	};

}

////////////////////////////////////////////////////////////////////////////////////////////
// DxrMethod namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxrMethod {

	UINT Alignment(size_t size, UINT align);

	UINT WriteShaderIdentifier(void* dst, const void* shaderId);

	UINT WriteGPUDescriptor(void* dst, const DxObject::Descriptor& descriptor);

	uint8_t* WriteShaderRecord(
		uint8_t* dst,
		DxrObject::BottomLevelAS* blas , UINT recordSize,
		ID3D12StateObjectProperties* prop
	);

}
