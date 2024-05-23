#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <DxDevices.h>
#include <DxDescriptorHeaps.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxrObject{

	////////////////////////////////////////////////////////////////////////////////////////////
	// ResultBuffer class
	////////////////////////////////////////////////////////////////////////////////////////////
	class ResultBuffer { //!< DxrSystem
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init(
			DxObject::Devices* devices, DxObject::DescriptorHeaps* descriptorHeaps,
			int32_t clientWidth, int32_t clientHeight
		);

		void Term();

		const DxObject::Descriptor& GetDescriptorUAV() const { return descriptorUAV_; }

		ID3D12Resource* GetResource() const { return resource_.Get(); }

		const D3D12_GPU_DESCRIPTOR_HANDLE& GetSRVHandleGPU() const { return descriptorSRV_.handleGPU; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		DxObject::DescriptorHeaps* descriptorHeaps_ = nullptr;

		ComPtr<ID3D12Resource> resource_;
		DxObject::Descriptor   descriptorUAV_;
		DxObject::Descriptor   descriptorSRV_;

	};

}