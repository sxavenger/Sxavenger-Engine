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
#include <deque>

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

	////////////////////////////////////////////////////////////////////////////////////////////
	// DescriptorHeaps class
	////////////////////////////////////////////////////////////////////////////////////////////
	class DescriptorHeaps {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief コンストラクタ
		//! 
		//! @param[in] devices DxObject::Devices
		DescriptorHeaps(Devices* devices);

		//! @brief デストラクタ
		~DescriptorHeaps();

		//! @brief 初期化処理
		//! 
		//! @param[in] devices DxObject::Devices
		void Init(Devices* devices);

		//! @brief 終了処理
		void Term();

		//! @brief 要素サイズを取得
		//! 
		//! @param[in] type  DescriptorType
		//! 
		//! @return 要素サイズを返却
		const uint32_t GetIndexSize(DescriptorType type) const {
			assert(type < DescriptorType::kDescriptorHeapCount);
			return descriptorIndexSize_[type];
		}

		////! @brief indexのDescrpitorの削除
		////! 
		////! @param[in] type  DescriptorType
		////! @param[in] index 削除するindex
		//void Erase(DescriptorType type, uint32_t index);

		//! @brief indexのDescrpitorの削除
		//!
		//! @param[in, out] descriptor
		void Erase(Descriptor& descriptor);

		Descriptor GetCurrentDescriptor(DescriptorType type);

		//! @brief DescriptorHeapを取得
		//! 
		//! @param[in] type  DescriptorType
		//! 
		//! @return DescriptorHeapを返却
		ID3D12DescriptorHeap* GetDescriptorHeap(DescriptorType type) const {
			assert(type < DescriptorType::kDescriptorHeapCount);
			return descriptorHeaps_[type].Get();
		}

		// use debug
		const uint32_t GetIndexCount(DescriptorType type) {
			assert(type < DescriptorType::kDescriptorHeapCount);
			return descriptorIndexCount_[type];
		}

		const std::deque<uint32_t>& GetDescriptorVacantQueue(DescriptorType type) {
			assert(type < DescriptorType::kDescriptorHeapCount);
			return descriptorVacantIndexs_[type];
		}

		const uint32_t GetUsedDescriptor(DescriptorType type) {
			assert(type < DescriptorType::kDescriptorHeapCount);
			return static_cast<uint32_t>(descriptorIndexCount_[type] - descriptorVacantIndexs_[type].size());
		}

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		ComPtr<ID3D12DescriptorHeap> descriptorHeaps_[DescriptorType::kDescriptorHeapCount];
		uint32_t                     descriptorSize_[DescriptorType::kDescriptorHeapCount]; // constにしたい

		uint32_t descriptorIndexSize_[DescriptorType::kDescriptorHeapCount];
		uint32_t descriptorIndexCount_[DescriptorType::kDescriptorHeapCount];

		std::deque<uint32_t> descriptorVacantIndexs_[DescriptorType::kDescriptorHeapCount]; //!< 動的テクスチャの隙間を埋めるため

		//=========================================================================================
		// private methods
		//=========================================================================================

		//! @breif 使用できるDescriptorsのindexを取得
		//! 
		//! @param[in] type DescriptorType
		//! 
		//! @return 使用できるDescriptorsのindexを返却
		const uint32_t GetDescriptorCurrentIndex(DescriptorType type);

		//! @brief CPUDescriptorHandleの取得
		//! 
		//! @param[in] type  DescriptorType
		//! @param[in] index
		//! 
		//! @return CPUDescriptorHandleの返却
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(DescriptorType type, uint32_t index) const {
			assert(type < DescriptorType::kDescriptorHeapCount);

			return DxObjectMethod::GetCPUDescriptorHandle(
				descriptorHeaps_[type].Get(),
				descriptorSize_[type],
				index
			);
		}

		//! @brief GPUDescriptorHandleの取得
		//! 
		//! @param[in] type  DescriptorType
		//! @param[in] index
		//! 
		//! @return GPUDescriptorHandleの返却
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(DescriptorType type, uint32_t index) const {
			assert(type < DescriptorType::kDescriptorHeapCount);

			return DxObjectMethod::GetGPUDescriptorHandle(
				descriptorHeaps_[type].Get(),
				descriptorSize_[type],
				index
			);
		}
	};

}