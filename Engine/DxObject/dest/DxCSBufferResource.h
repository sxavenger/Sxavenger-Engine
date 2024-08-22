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

		//! @brief writeResourceをuavResourceにコピー
		void Write(ID3D12GraphicsCommandList* commandList);

		//! @brief uavResourceをreadBackResourceにコピー
		void ReadBack(ID3D12GraphicsCommandList* commandList);

		const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
			return uavResource_->GetGPUVirtualAddress();
		}

		//! @brief VertexBufferを取得
		//! 
		//! @return VertexBufferを返却
		virtual const D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const {
			D3D12_VERTEX_BUFFER_VIEW result = {};
			result.BufferLocation = GetGPUVirtualAddress();
			result.SizeInBytes    = structureSize_ * indexSize_;
			result.StrideInBytes  = structureSize_;

			return result;
		}

		const uint32_t GetIndexSize() const { return indexSize_; }

		const UINT GetStructureSize() const { return structureSize_; }

		ID3D12Resource* GetResource() const { return uavResource_.Get(); }

	protected:

		//=========================================================================================
		// protected variables
		//=========================================================================================

		/* resources */

		ComPtr<ID3D12Resource> uavResource_;       //!< RW用のResource
		ComPtr<ID3D12Resource> writeResource_;     //!< 書き込み用のResource
		ComPtr<ID3D12Resource> readBackResource_;  //!< 読み取り用のResource

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

		CSBufferResource() = delete;

		CSBufferResource(Devices* devices, uint32_t indexSize)
			: CSBaseBufferResource(devices, indexSize, sizeof(T)) {
			Init();
		}

		void Init();

		/* data関係 */

		void SetWriteData(uint32_t index, const T& value) {
			assert(index < indexSize_);
			writeDataArray_[index] = value;
		}

		const T& GetReadBackData(uint32_t index) {
			assert(index < indexSize_);
			return readBackDataArray_[index];
		}

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		T* writeDataArray_;
		T* readBackDataArray_;
		//!< のちに一つにするかも
		// todo: std::spanを利用する
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
	writeResource_->Map(0, nullptr, reinterpret_cast<void**>(&writeDataArray_));
	readBackResource_->Map(0, nullptr, reinterpret_cast<void**>(&readBackDataArray_));
}