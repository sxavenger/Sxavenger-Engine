#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>

// c++
#include <cstdint>
#include <cassert>

// c++
#include <DxObjectMethod.h>

// ComPtr
#include <ComPtr.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObject {

	//-----------------------------------------------------------------------------------------
	// DxObject forward
	//-----------------------------------------------------------------------------------------
	class Devices;
	class DescriptorHeaps;
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// DepthStencil class
	////////////////////////////////////////////////////////////////////////////////////////////
	class DepthStencil {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief コンストラクタ
		//! 
		//! @param[in] devices         DxObject::Devices
		//! @param[in] descriptorHeaps DxObject::DescriptorHeaps
		//! @param[in] clientWidth     クライアント領域横幅
		//! @param[in] clientHeight    クライアント領域縦幅
		DepthStencil(
			Devices* devices, DescriptorHeaps* descriptorHeaps,
			int32_t clientWidth, int32_t clientHeight
		);

		//! @brief デストラクタ
		~DepthStencil();

		//! @brief 初期化処理
		//! 
		//! @param[in] devices         DxObject::Devices
		//! @param[in] descriptorHeaps DxObject::DescriptorHeaps
		//! @param[in] clientWidth     クライアント領域横幅
		//! @param[in] clientHeight    クライアント領域縦幅
		void Init(
			Devices* devices, DescriptorHeaps* descriptorHeaps,
			int32_t clientWidth, int32_t clientHeight
		);

		//! @brief 終了処理
		void Term();

		const D3D12_CPU_DESCRIPTOR_HANDLE& GetHandle() const { return descriptorDSV_.GetCPUHandle(); }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		ComPtr<ID3D12Resource> depthStencilResource_;

		DxObject::Descriptor descriptorDSV_;

	};

}