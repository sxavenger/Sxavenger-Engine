#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"

//* c++
#include <optional>
#include <span>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BaseDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseDimensionBuffer()  = default;
	~BaseDimensionBuffer() { Release(); }

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	const uint32_t GetSize() const { return size_; }

	const uint32_t GetStride() const { return stride_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12Resource> resource_;

	std::optional<D3D12_GPU_VIRTUAL_ADDRESS> address_ = std::nullopt;

	//* paraemter *//

	uint32_t size_   = NULL;
	uint32_t stride_ = NULL;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void Create(Device* device, uint32_t size, size_t stride);

	void Release();

	bool CheckIndex(uint32_t index);

};

////////////////////////////////////////////////////////////////////////////////////////////
// DimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class DimensionBuffer
	: public BaseDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DimensionBuffer()  = default;
	~DimensionBuffer() { Release(); }

	void Create(Device* device, uint32_t size);

	void Release();

	const T& At(uint32_t index) const;

	const T* GetData();

	void Memcpy(const T* value);

	const std::span<T>& GetMappedData() const { return mappedDatas_; }

	//=========================================================================================
	// operator
	//=========================================================================================

	T& operator[](uint32_t index);
	const T& operator[](uint32_t index) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::span<T> mappedDatas_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// VertexDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class VertexDimensionBuffer
	: public DimensionBuffer<T> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	const D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// IndexDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class IndexDimensionBuffer
	: public DimensionBuffer<UINT> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	const D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// DimensionBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
inline void DimensionBuffer<T>::Create(Device* device, uint32_t size) {
	BaseDimensionBuffer::Create(device, size, sizeof(T));

	T* mappingTarget = nullptr;

	// resourceをマッピング
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappingTarget));

	mappedDatas_ = { mappingTarget, size_ };
}

template<class T>
inline void DimensionBuffer<T>::Release() {
	BaseDimensionBuffer::Release();
	mappedDatas_ = {};
}

template<class T>
inline const T& DimensionBuffer<T>::At(uint32_t index) const {
	Assert(CheckIndex(index), "Dimension Buffer out of range.");
	return mappedDatas_[index];
}

template<class T>
inline const T* DimensionBuffer<T>::GetData() {
	return mappedDatas_.data();
}

template<class T>
inline void DimensionBuffer<T>::Memcpy(const T* value) {
	std::memcpy(mappedDatas_.data(), value, stride_ * size_);
}

template<class T>
inline T& DimensionBuffer<T>::operator[](uint32_t index) {
	Assert(CheckIndex(index), "Dimension Buffer out of range.");
	return mappedDatas_[index];
}

template<class T>
inline const T& DimensionBuffer<T>::operator[](uint32_t index) const {
	Assert(CheckIndex(index), "Dimension Buffer out of range.");
	return mappedDatas_[index];
}

////////////////////////////////////////////////////////////////////////////////////////////
// VertexDimensionBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
inline const D3D12_VERTEX_BUFFER_VIEW VertexDimensionBuffer<T>::GetVertexBufferView() const {
	D3D12_VERTEX_BUFFER_VIEW result = {};
	result.BufferLocation = this->GetGPUVirtualAddress();
	result.SizeInBytes    = this->stride_ * this->size_;
	result.StrideInBytes  = this->stride_;

	return result;
}

_DXOBJECT_NAMESPACE_END
