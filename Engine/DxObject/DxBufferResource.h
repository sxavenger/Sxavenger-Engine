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
namespace DxObject {

	////////////////////////////////////////////////////////////////////////////////////////////
	// BufferIndex class
	////////////////////////////////////////////////////////////////////////////////////////////
	class BufferIndex {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @breif コンストラクタ
		//! 
		//! @param[in] indexSize 配列サイズ
		BufferIndex(uint32_t indexSize) { Init(indexSize); }

		//! @brief デストラクタ
		~BufferIndex() { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] indexSize 配列サイズ
		void Init(uint32_t indexSize);

		//! @breif 終了処理
		void Term();

		//! @breif 配列のサイズを獲得
		const uint32_t GetSize() const { return indexSize_; }

	protected:

		//=========================================================================================
		// protected variables
		//=========================================================================================

		ComPtr<ID3D12Resource> resource_;

		uint32_t indexSize_;

		//=========================================================================================
		// protected methods
		//=========================================================================================
		
		//! @brief indexSizeより小さいか
		//! 
		//! @param[in] index
		//! 
		//! @retval true  正常終了
		//! @retval false indexSizeを超過
		bool CheckIndex(uint32_t index);

		BufferIndex() { indexSize_ = NULL; }

	};


	////////////////////////////////////////////////////////////////////////////////////////////
	// BufferResource class
	////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	class BufferResource
		: public BufferIndex {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @breif コンストラクタ
		//! 
		//! @param[in] devices   DxObject::Devices
		//! @param[in] indexSize 配列サイズ
		BufferResource(DxObject::Devices* devices, uint32_t indexSize)
			: BufferIndex(indexSize) { Init(devices); }

		//! @brief デストラクタ
		~BufferResource() { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] devices DxObject::Devices
		void Init(DxObject::Devices* devices);

		//! @brief 終了処理
		void Term();

		//! @brief BufferResourceを取得
		//! 
		//! @return BufferResourceを返却
		ID3D12Resource* GetResource() const {
			return resource_.Get();
		}

		//! @brief GPUAddressを取得
		//! 
		//! @return GPUAddressを返却
		const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
			return resource_->GetGPUVirtualAddress();
		}

		//! @brief VertexBufferを取得
		//! 
		//! @return VertexBufferを返却
		const D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const {
			D3D12_VERTEX_BUFFER_VIEW result = {};
			result.BufferLocation = resource_->GetGPUVirtualAddress();
			result.SizeInBytes    = sizeof(T) * indexSize_;
			result.StrideInBytes  = sizeof(T);

			return result;
		}

		void Memcpy(const T* value) {
			memcpy(dataArray_, value, sizeof(T) * indexSize_);
		}

		//=========================================================================================
		// operator
		//=========================================================================================

		T& operator[](uint32_t index) {
			CheckIndex(index);

			return dataArray_[index];
		}

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		T* dataArray_;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// BufferPtrResource class
	////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	class BufferPtrResource
		: public BufferIndex {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @breif コンストラクタ
		//! 
		//! @param[in] devices   DxObject::Devices
		//! @param[in] indexSize 配列サイズ
		BufferPtrResource(DxObject::Devices* devices, uint32_t indexSize)
			: BufferIndex(indexSize) { Init(devices); }

		//! @brief デストラクタ
		~BufferPtrResource() { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] devices DxObject::Devices
		void Init(DxObject::Devices* devices);

		//! @brief 終了処理
		void Term();

		//! @brief GPUAddressを取得
		//! 
		//! @return GPUAddressを返却
		const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() {
			LoadPtrData();
			return resource_->GetGPUVirtualAddress();
		}

		//! @brief VertexBufferを取得
		//! 
		//! @return VertexBufferを返却
		const D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() {
			LoadPtrData();

			D3D12_VERTEX_BUFFER_VIEW result = {};
			result.BufferLocation = resource_->GetGPUVirtualAddress();
			result.SizeInBytes    = sizeof(T) * indexSize_;
			result.StrideInBytes  = sizeof(T);

			return result;
		}

		//! @brief dataPtrArrayにvalueを設定
		//! 
		//! @param[in] index 要素数
		//! @param[in] value データ
		void SetPtr(uint32_t index, T* value) {
			if (!CheckIndex(index)) {
				return;
			}

			dataPtrArray_[index] = value;
		}

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		T* dataArray_;
		std::vector<T*> dataPtrArray_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		void LoadPtrData();
	};

	
	////////////////////////////////////////////////////////////////////////////////////////////
	// IndexBufferResource class
	////////////////////////////////////////////////////////////////////////////////////////////
	class IndexBufferResource
		: public BufferIndex {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @breif コンストラクタ
		//! 
		//! @param[in] devices      DxObject::Devices
		//! @param[in] vertexBuffer DxObject::BufferResource || DxObject::BufferPtrResorce の vertexBuffer
		IndexBufferResource(DxObject::Devices* devices, const BufferIndex* vertexBuffer);

		//! @breif コンストラクタ
		//! 
		//! @param[in] devices   DxObject::Devices
		//! @param[in] indexSize 配列サイズ
		IndexBufferResource(DxObject::Devices* devices, uint32_t indexSize)
			: BufferIndex(indexSize) { Init(devices); }

		//! @brief デストラクタ
		~IndexBufferResource() { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] devices DxObject::Devices
		void Init(DxObject::Devices* devices);

		//! @brief 終了処理
		void Term();

		//! @brief IndexBufferを取得
		//! 
		//! @return IndexBufferを返却
		const D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const {
			D3D12_INDEX_BUFFER_VIEW result = {};
			result.BufferLocation = resource_->GetGPUVirtualAddress();
			result.SizeInBytes    = sizeof(uint32_t) * indexSize_;
			result.Format         = DXGI_FORMAT_R32_UINT;

			return result;
		}

		//! @brief dataArrayにvalueを設定
		//! 
		//! @param[in] index 要素数
		//! @param[in] value データ
		void Set(uint32_t index, uint32_t value) {
			if (!CheckIndex(index)) {
				return;
			}

			dataArray_[index] = value;
		}

		void Memcpy(const uint32_t* value) {
			memcpy(dataArray_, value, sizeof(uint32_t) * indexSize_);
		}

		//=========================================================================================
		// operator
		//=========================================================================================

		uint32_t& operator[](uint32_t index) {
			CheckIndex(index);

			return dataArray_[index];
		}

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		uint32_t* dataArray_;

		uint32_t kMaxTriangleCount_;
	};

}

////////////////////////////////////////////////////////////////////////////////////////////
// BufferResource class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void DxObject::BufferResource<T>::Init(DxObject::Devices* devices) {

	// deviceを取り出す
	ID3D12Device* device = devices->GetDevice();

	// 配列分のBufferResourceを生成
	resource_ = DxObjectMethod::CreateBufferResource(
		device,
		sizeof(T) * indexSize_
	);

	// resourceをマッピング
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&dataArray_));
}

template<typename T>
void DxObject::BufferResource<T>::Term() {
	resource_->Release();
}

////////////////////////////////////////////////////////////////////////////////////////////
// BufferPtrResource class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void DxObject::BufferPtrResource<T>::Init(DxObject::Devices* devices) {
	// deviceを取り出す
	ID3D12Device* device = devices->GetDevice();

	// 配列分のBufferResourceを生成
	resource_ = DxObjectMethod::CreateBufferResource(
		device,
		sizeof(T) * indexSize_
	);

	// resourceをマッピング
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&dataArray_));

	// ptrArrayの配列数を動的生成
	dataPtrArray_.resize(indexSize_);
}

template<typename T>
void DxObject::BufferPtrResource<T>::Term() {
	resource_->Release();
	dataPtrArray_.clear();
}

template<typename T>
void DxObject::BufferPtrResource<T>::LoadPtrData() {
	memcpy(dataArray_, *dataPtrArray_.data(), sizeof(T) * indexSize_);
}