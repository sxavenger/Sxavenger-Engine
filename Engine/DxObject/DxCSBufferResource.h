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
#include <vector>
#include <memory>

// DxObject
#include <DxObjectMethod.h>
#include <DxDevices.h>

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
namespace DxObject { //!< DxSource, use Compute Shader

	////////////////////////////////////////////////////////////////////////////////////////////
	// CSBaseBufferResource class
	////////////////////////////////////////////////////////////////////////////////////////////
	class CSBaseBufferResource {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		CSBaseBufferResource(Devices* devices, uint32_t indexSize, size_t structureSize) {
			Init(devices, indexSize, structureSize);
		}

		~CSBaseBufferResource() { Term(); }

		//! @brief 初期化処理
		//! @brief sizeの保存, resourceの生成
		void Init(Devices* devices, uint32_t indexSize, size_t structureSize);

		void Term();

		//! @brief 現在のuavResourceをreadBackResourceにコピー
		void CopyReadBack(ID3D12GraphicsCommandList* commandList);

		const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddressUAV() const {
			return uavResource_->GetGPUVirtualAddress();
		}

		const uint32_t GetIndexSize() const { return indexSize_; }

		const UINT GetStructureSize() const { return structureSize_; }

	protected:

		//=========================================================================================
		// protected variables
		//=========================================================================================

		/* resources */

		ComPtr<ID3D12Resource> uavResource_;
		ComPtr<ID3D12Resource> readBackResource_;

		/* parameter */

		uint32_t indexSize_;     //!< 配列のサイズ
		UINT     structureSize_; //!< 構造体のサイズ

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// CSBufferResource class
	////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	class CSBufferResource
		: public CSBaseBufferResource {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		CSBufferResource(Devices* devices, uint32_t indexSize)
			: CSBaseBufferResource(devices, indexSize, sizeof(T)) {
			Init();
		}

		void Init();

		const T& GetData(uint32_t index) {
			assert(index < indexSize_);
			return readBackDataArray_[index];
		}

		T* GetDataArray() const { return readBackDataArray_; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		T* readBackDataArray_;

	};

}

////////////////////////////////////////////////////////////////////////////////////////////
// DxMethods namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxMethods {

	ComPtr<ID3D12Resource> CreateResource(
		ID3D12Device* device,
		D3D12_HEAP_TYPE heapType,
		size_t sizeInBytes,
		D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state
	);

}

/* template class methods */

////////////////////////////////////////////////////////////////////////////////////////////
// CSBufferResource class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline void DxObject::CSBufferResource<T>::Init() {
	// mapping処理の記述
	readBackResource_->Map(0, nullptr, reinterpret_cast<void**>(&readBackDataArray_));
}