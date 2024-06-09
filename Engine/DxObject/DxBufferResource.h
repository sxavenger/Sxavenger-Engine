#pragma once
//*****************************************************************************************
// BaseBufferResource:                don't use
// BufferResource, BufferPtrResource: ConstantBuffer, IASetVertexBuffer
// IndexBufferResouruce:              IASetIndexBuffer
// 
// buffers is use DxObject::StructuredBuffer
//*****************************************************************************************

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
namespace DxObject {

	////////////////////////////////////////////////////////////////////////////////////////////
	// BaseBufferResource class
	////////////////////////////////////////////////////////////////////////////////////////////
	class BaseBufferResource { //!< Baseクラスなのでこのクラス単体では使わない
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @breif コンストラクタ
		//!
		//! @param[in] indexSize     配列のサイズ
		//! @param[in] structureSize 構造体のサイズ
		BaseBufferResource(uint32_t indexSize, size_t structureSize) {
			Init(indexSize, structureSize);
		}

		//! @brief デストラクタ
		~BaseBufferResource() { Term(); }

		//! @breif 初期化処理
		//! 
		//! @param[in] indexSize     配列のサイズ
		//! @param[in] structureSize 構造体のサイズ
		void Init(uint32_t indexSize, size_t structureSize);

		//! @breif 終了処理
		void Term();

		//! @breif 配列のサイズを取得
		virtual const uint32_t GetIndexSize() const { return indexSize_; }

		//! @breif 構造体のサイズを取得
		const UINT GetStructureSize() const { return structureSize_; }

		//! @brief GPUAddressを取得
		//! 
		//! @return GPUAddressを返却
		virtual const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() {
			return resource_->GetGPUVirtualAddress();
		}

		//! @brief Resourceの取得
		virtual ID3D12Resource* GetResource() { return resource_.Get(); }

	protected:

		//=========================================================================================
		// protected variables
		//=========================================================================================

		ComPtr<ID3D12Resource> resource_;

		uint32_t indexSize_;     //!< 配列のサイズ
		UINT     structureSize_; //!< 構造体のサイズ

		//=========================================================================================
		// protected methods
		//=========================================================================================

		//! @brief 要素数がindexSize以上でないかの確認
		//! 
		//! @retval true  配列サイズ以下
		//! @retval false 配列サイズ以上
		virtual bool CheckElementCount(uint32_t elementCount);

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// BufferResource class
	////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	class BufferResource
		: public BaseBufferResource {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @breif コンストラクタ
		//! 
		//! @param[in] devices   DxObject::Devices
		//! @param[in] indexSize 配列サイズ
		BufferResource(Devices* devices, uint32_t indexSize)
			: BaseBufferResource(indexSize, sizeof(T)) {
			Init(devices);
		}

		//! @brief デストラクタ
		~BufferResource() { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] devices DxObject::Devices
		void Init(Devices* devices);

		//! @brief 終了処理
		void Term();

		//! @brief VertexBufferを取得
		//! 
		//! @return VertexBufferを返却
		const D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() {
			D3D12_VERTEX_BUFFER_VIEW result = {};
			result.BufferLocation = GetGPUVirtualAddress();
			result.SizeInBytes    = structureSize_ * indexSize_;
			result.StrideInBytes  = structureSize_;

			return result;
		}

		void Memcpy(const T* value) {
			memcpy(dataArray_, value, structureSize_ * indexSize_);
		}

		//=========================================================================================
		// operator
		//=========================================================================================

		T& operator[](uint32_t element) {
			CheckElementCount(element);

			return dataArray_[element];
		}

		//
		// test functions
		//

		T* GetDataArray() const {
			return dataArray_;
		}

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		T* dataArray_; //!< mappingするデータ

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// BufferPtrResource class
	////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	class BufferPtrResource
		: public BaseBufferResource {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @breif コンストラクタ
		//! 
		//! @param[in] devices   DxObject::Devices
		//! @param[in] indexSize 配列サイズ
		BufferPtrResource(Devices* devices, uint32_t indexSize)
			: BaseBufferResource(indexSize, sizeof(T)) {
			Init(devices);
		}

		//! @brief デストラクタ
		~BufferPtrResource() { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] devices DxObject::Devices
		void Init(Devices* devices);

		//! @brief 終了処理
		void Term();

		//! @brief dataPtrArrayにvalueを設定
		//! 
		//! @param[in] element 要素数
		//! @param[in] value データ
		void SetPtr(uint32_t element, T* value) {
			if (!CheckElementCount(element)) {
				assert(false); //!< 範囲外参照
				return;
			}

			dataPtrArray_.at(element) = value;
		}

		const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() override {
			LoadPtrData();
			return resource_->GetGPUVirtualAddress();
		}

		//! @brief VertexBufferを取得
		//! 
		//! @return VertexBufferを返却
		const D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() {
			D3D12_VERTEX_BUFFER_VIEW result = {};
			result.BufferLocation = GetGPUVirtualAddress();
			result.SizeInBytes    = structureSize_ * indexSize_;
			result.StrideInBytes  = structureSize_;

			return result;
		}

		//! @brief Resourceの取得
		virtual ID3D12Resource* GetResource() override {
			LoadPtrData();
			return resource_.Get();
		}

		//
		// Test functions
		//

		const T* GetDataArray() {
			LoadPtrData();
			return dataArray_;
		}

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		T*              dataArray_;
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
		: public BaseBufferResource {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @breif コンストラクタ
		//! 
		//! @param[in] devices   DxObject::Devices
		//! @param[in] indexSize 配列サイズ
		IndexBufferResource(Devices* devices, uint32_t indexSize)
			: BaseBufferResource(indexSize, sizeof(uint32_t)) {
			Init(devices);
		}

		//! @brief デストラクタ
		~IndexBufferResource() { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] devices DxObject::Devices
		void Init(Devices* devices);

		//! @brief 終了処理
		void Term();

		//! @brief IndexBufferを取得
		//! 
		//! @return IndexBufferを返却
		const D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const {
			D3D12_INDEX_BUFFER_VIEW result = {};
			result.BufferLocation = resource_->GetGPUVirtualAddress();
			result.SizeInBytes    = structureSize_ * indexSize_;
			result.Format         = DXGI_FORMAT_R32_UINT;

			return result;
		}

		void Memcpy(const uint32_t* value) {
			memcpy(dataArray_, value, sizeof(uint32_t) * indexSize_);
		}

		//=========================================================================================
		// operator
		//=========================================================================================

		uint32_t& operator[](uint32_t element) {
			CheckElementCount(element);

			return dataArray_[element];
		}

		//
		// Test funcitons
		//

		uint32_t* GetDataArray() const {
			return dataArray_;
		}

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		uint32_t* dataArray_;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// DynamicBufferResource class
	////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	class DynamicBufferResource
		: public BaseBufferResource { //!< テスト段階なのでstructuredBufferなどとして使わないように
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @breif コンストラクタ
		//! 
		//! @param[in] devices   DxObject::Devices
		//! @param[in] indexSize 配列サイズ
		DynamicBufferResource(Devices* devices)
			: BaseBufferResource(0, sizeof(T)) {
			Init(devices);
		}

		//! @brief デストラクタ
		~DynamicBufferResource() { Term(); }

		//! @brief 初期化処理
		//! 
		//! @param[in] devices DxObject::Devices
		void Init(Devices* devices);

		//! @brief 終了処理
		void Term();

		//! @brief 要素の全削除
		void Clear();

		//! @brief 配列の最大数を返却
		const uint32_t GetIndexSize() const override { return kMaxIndexSize_; }

		const uint32_t GetCurrentIndexSize() const { return indexSize_; }

		void Memcpy(const T* value, uint32_t indexSize) {
			CheckElementCount(indexSize);
			memcpy(dataArray_, value, structureSize_ * indexSize);
		}

		//=========================================================================================
		// operator
		//=========================================================================================

		T& operator[](uint32_t element) {
			CheckElementCount(element);

			return dataArray_[element];
		}

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		static const uint32_t kMaxIndexSize_ = 200; //!< 不足した場合, 増やす

		T* dataArray_;

		/* 用途の変更
		 indexSize -> 現在のindexの最大値
		*/

		ID3D12Device* device_;

		//=========================================================================================
		// protected methods
		//=========================================================================================

		//! @brief 要素数がindexSize以上でないかの確認
		//! 
		//! @retval true  配列サイズ以下
		//! @retval false 配列サイズ以上
		bool CheckElementCount(uint32_t elementCount) override;

	};

}

////////////////////////////////////////////////////////////////////////////////////////////
// BufferResource class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline void DxObject::BufferResource<T>::Init(Devices* devices) {

	// deviceを取り出す
	ID3D12Device* device = devices->GetDevice();

	// 配列分のBufferResourceを生成
	resource_ = DxObjectMethod::CreateBufferResource(
		device,
		structureSize_ * indexSize_
	);

	// resourceをマッピング
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&dataArray_));
}

template<typename T>
inline void DxObject::BufferResource<T>::Term() {
}

////////////////////////////////////////////////////////////////////////////////////////////
// BufferPtrResource class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline void DxObject::BufferPtrResource<T>::Init(Devices* devices) {

	// deviceを取り出す
	ID3D12Device* device = devices->GetDevice();

	dataPtrArray_.resize(indexSize_);

	// 配列分のBufferResourceを生成
	resource_ = DxObjectMethod::CreateBufferResource(
		device,
		structureSize_ * indexSize_
	);

	// resourceをマッピング
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&dataArray_));
}

template<typename T>
inline void DxObject::BufferPtrResource<T>::Term() {
	dataPtrArray_.clear();
}

template<typename T>
inline void DxObject::BufferPtrResource<T>::LoadPtrData() {
	memcpy(dataArray_, *dataPtrArray_.data(), structureSize_ * indexSize_);
}

////////////////////////////////////////////////////////////////////////////////////////////
// DynamicBufferResource class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline void DxObject::DynamicBufferResource<T>::Init(Devices* devices) {

	// deviceを取り出す
	device_ = devices->GetDevice();

	// 配列分のBufferResourceを生成
	resource_ = DxObjectMethod::CreateBufferResource(
		device_,
		structureSize_ * kMaxIndexSize_
	);

	// resourceをマッピング
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&dataArray_));

	Clear();
}

template<typename T>
inline void DxObject::DynamicBufferResource<T>::Term() {
}

template<typename T>
inline void DxObject::DynamicBufferResource<T>::Clear() {
	T t;
	std::fill(dataArray_, dataArray_ + kMaxIndexSize_, t);
	indexSize_ = 0;
}

template<typename T>
inline bool DxObject::DynamicBufferResource<T>::CheckElementCount(uint32_t elementCount) {
	if (elementCount >= kMaxIndexSize_) {
		return false;
	}

	// 最大数の更新
	if (elementCount >= indexSize_) {
		indexSize_ = elementCount + 1;
	}

	return true;
}
