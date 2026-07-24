#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxResource.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>

//* c++
#include <optional>
#include <span>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BaseDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 定数/構造化バッファ等の基底クラス. リソースと要素数/strideを共通管理する
class BaseDimensionBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Category enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief バッファのヒープ種別(用途)
	enum class Category {
		Default  = D3D12_HEAP_TYPE_DEFAULT,  //!< GPU専用.
		Upload   = D3D12_HEAP_TYPE_UPLOAD,   //!< CPU書き込み用.
		Readback = D3D12_HEAP_TYPE_READBACK, //!< CPU読み込み用.
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseDimensionBuffer(size_t stride) : stride_(stride) {}
	virtual ~BaseDimensionBuffer() { Reset(); }

	//* option *//

	//! @brief バッファを解放する
	void Reset();

	//! @brief デバッグ用のリソース名を設定する
	void SetName(const std::wstring& name) const { resource_.SetName(name); }

	//* getter *//

	//! @brief 内部のResourceラッパーを取得する
	DxObject::Resource& Get() { return resource_; }

	//! @brief 内部のID3D12Resourceを取得する
	ID3D12Resource* GetResource() const { return resource_.Get(); }

	//! @brief バッファのGPU仮想アドレスを取得する
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return resource_.GetGPUVirtualAddress(); }

	//! @brief 要素数を取得する
	const uint32_t GetSize() const { return size_; }

	//! @brief 1要素のバイトサイズ(stride)を取得する
	const size_t GetStride() const { return stride_; }

	//! @brief バッファ全体のバイトサイズを取得する
	const size_t GetByteSize() const { return size_ * stride_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* DirectX12 *//

	DxObject::Resource resource_;

	//* parameter *//

	uint32_t size_       = NULL;
	const size_t stride_ = NULL;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* helper create methods *//

	static D3D12_RESOURCE_STATES GetDefaultState(Category category);
	static D3D12_RESOURCE_FLAGS GetResourceFlags(Category category);

	void CreateBuffer(DxObject::Device* device, uint32_t size, Category category);

};

////////////////////////////////////////////////////////////////////////////////////////////
// ConstantBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief [ConstantBuffer] 1次元Bufferクラス.
template <typename T>
class ConstantBuffer
	: public BaseDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ConstantBuffer() : BaseDimensionBuffer(sizeof(T)) {}
	~ConstantBuffer() override { Unmap(); }

	void Create(DxObject::Device* device);

	//* getter *//

	T* GetData() const { return data_; }

	//* access option *//

	T& At();
	const T& At() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	T* data_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Map();

	void Unmap();

};

////////////////////////////////////////////////////////////////////////////////////////////
// DimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief [ConstantBuffer/StructuredBuffer] 1次元Bufferクラス.
template <typename T>
class DimensionBuffer
	: public BaseDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DimensionBuffer() : BaseDimensionBuffer(sizeof(T)) {}
	~DimensionBuffer() override { Unmap(); }

	void Create(DxObject::Device* device, uint32_t size);

	void Memcpy(const T* data);

	void Fill(const T& value);

	//* getter *//

	T* GetData() const { return data_.data(); }

	const std::span<T>& GetSpan() const { return data_; }

	//* access option *//

	T& At(size_t index);
	const T& At(size_t index) const;

	T& operator[](size_t index) { return At(index); }
	const T& operator[](size_t index) const { return At(index); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::span<T> data_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Map();

	void Unmap();

};

////////////////////////////////////////////////////////////////////////////////////////////
// ConstantBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void ConstantBuffer<T>::Create(DxObject::Device* device) {
	BaseDimensionBuffer::CreateBuffer(device, 1, Category::Upload);
	resource_.SetName(L"Constant Buffer");

	Map();
}

template <typename T>
inline T& ConstantBuffer<T>::At() {
	SXAVENGER_ENGINE StreamLogger::AssertA(data_ != nullptr, "Constant Buffer not mapped.");
	return *data_;
}

template <typename T>
inline const T& ConstantBuffer<T>::At() const {
	SXAVENGER_ENGINE StreamLogger::AssertA(data_ != nullptr, "Constant Buffer not mapped.");
	return *data_;
}

template <typename T>
inline void ConstantBuffer<T>::Map() {
	//!< resourceをマッピング
	resource_.Map(reinterpret_cast<void**>(&data_));
}

template <typename T>
inline void ConstantBuffer<T>::Unmap() {
	if (resource_ != nullptr) {
		resource_.Unmap();
	}

	data_ = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// DimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void DimensionBuffer<T>::Create(DxObject::Device* device, uint32_t size) {
	BaseDimensionBuffer::CreateBuffer(device, size, Category::Upload);
	resource_.SetName(L"Dimension Buffer");

	Map();
}

template <typename T>
inline void DimensionBuffer<T>::Map() {
	T* ptr = nullptr;

	//!< resourceをマッピング
	resource_.Map(reinterpret_cast<void**>(&ptr));
	data_ = std::span<T>(ptr, size_);
}

template <typename T>
inline void DimensionBuffer<T>::Unmap() {
	if (resource_ != nullptr) {
		resource_.Unmap();
	}

	data_ = {};
}

template <typename T>
inline void DimensionBuffer<T>::Memcpy(const T* data) {
	std::memcpy(data_.data(), data, BaseDimensionBuffer::GetByteSize());
}

template <typename T>
inline void DimensionBuffer<T>::Fill(const T& value) {
	std::fill(data_.begin(), data_.end(), value);
}

template <typename T>
inline T& DimensionBuffer<T>::At(size_t index) {
	SXAVENGER_ENGINE StreamLogger::AssertA(index < size_, "Dimension Buffer out of range.");
	return data_[index];
}

template <typename T>
inline const T& DimensionBuffer<T>::At(size_t index) const {
	SXAVENGER_ENGINE StreamLogger::AssertA(index < size_, "Dimension Buffer out of range.");
	return data_[index];
}

DXOBJECT_NAMESPACE_END
