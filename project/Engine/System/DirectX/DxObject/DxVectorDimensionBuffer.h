#pragma once

#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxDimensionBuffer.h"

//* c++
#include <optional>
#include <span>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BaseVectorDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseVectorDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseVectorDimensionBuffer(size_t stride) : stride_(stride) {}
	~BaseVectorDimensionBuffer() { Release(); }

	virtual void Release();

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	const uint32_t GetSize() const { return size_; }

	const uint32_t GetCapacity() const { return capacity_; }

	const size_t GetStride() const { return stride_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12Resource> resource_;
	std::optional<D3D12_GPU_VIRTUAL_ADDRESS> address_ = std::nullopt;

	//* paraemter *//

	uint32_t size_       = NULL;
	uint32_t capacity_   = NULL;
	const size_t stride_ = NULL;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void UpdateAddress();

	void Create(Device* device, uint32_t capacity);

	bool CheckIndex(size_t index) const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// VectorDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class VectorDimensionBuffer
	: public BaseVectorDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	VectorDimensionBuffer() : BaseVectorDimensionBuffer(sizeof(T)) {}
	~VectorDimensionBuffer() { Release(); }

	void Resize(Device* device, uint32_t size);

	void Reserve(Device* device, uint32_t capacity);

	const T& At(size_t index) const;
	T& At(size_t index);

	//=========================================================================================
	// public operator
	//=========================================================================================

	const T& operator[](size_t index) const;
	T& operator[](size_t index);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::span<T> mappedDatas_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Map();

	void Recreate(Device* device, uint32_t capacity);

};

////////////////////////////////////////////////////////////////////////////////////////////
// VectorDimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
inline void VectorDimensionBuffer<T>::Resize(Device* device, uint32_t size) {
	// 引数の保存
	size_ = size;

	if (resource_ == nullptr || size > capacity_) {
		Recreate(device, size);

	} else {
		Map();
	}
}

template <class T>
inline void VectorDimensionBuffer<T>::Reserve(Device* device, uint32_t capacity) {
	if (capacity <= capacity_) {
		return;
	}

	Recreate(device, capacity);
}

template <class T>
inline const T& VectorDimensionBuffer<T>::At(size_t index) const {
	Assert(CheckIndex(index), "index out of range.");
	return mappedDatas_[index];
}

template <class T>
inline T& VectorDimensionBuffer<T>::At(size_t index) {
	Assert(CheckIndex(index), "index out of range.");
	return mappedDatas_[index];
}

template <class T>
inline const T& VectorDimensionBuffer<T>::operator[](size_t index) const {
	Assert(CheckIndex(index), "index out of range.");
	return mappedDatas_[index];
}

template <class T>
inline T& VectorDimensionBuffer<T>::operator[](size_t index) {
	Assert(CheckIndex(index), "index out of range.");
	return mappedDatas_[index];
}

template <class T>
inline void VectorDimensionBuffer<T>::Map() {
	T* mappingTarget = nullptr;

	// resourceをマッピング
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappingTarget));
	mappedDatas_ = { mappingTarget, size_ };
}

template <class T>
inline void VectorDimensionBuffer<T>::Recreate(Device* device, uint32_t capacity) {
	// 一時的に保存
	std::vector<T> tmp(mappedDatas_.begin(), mappedDatas_.end());
	tmp.resize(size_);

	// resourceの生成
	BaseVectorDimensionBuffer::Create(device, capacity);
	Map();

	// データのコピー
	std::copy(tmp.begin(), tmp.begin() + size_, mappedDatas_.begin());
	std::fill(mappedDatas_.begin() + size_, mappedDatas_.end(), T{});
}

_DXOBJECT_NAMESPACE_END
