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

	BaseDimensionBuffer(size_t stride) : stride_(stride) {}
	virtual ~BaseDimensionBuffer() { Release(); }

	void Release();

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	const uint32_t GetSize() const { return size_; }

	const uint32_t& GetDimension() const { return size_; }

	const size_t GetStride() const { return stride_; }

	const size_t GetByteSize() const { return size_ * stride_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12Resource> resource_;
	std::optional<D3D12_GPU_VIRTUAL_ADDRESS> address_ = std::nullopt;

	//* paraemter *//

	uint32_t size_       = NULL;
	const size_t stride_ = NULL;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void Create(Device* devices, uint32_t size);

	bool CheckIndex(size_t index) const;

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

	DimensionBuffer() : BaseDimensionBuffer(sizeof(T)) {}
	~DimensionBuffer() override { Unmap(); }

	void Create(Device* device, uint32_t size);

	void Map();

	void Unmap();

	T& At(size_t index);
	const T& At(size_t index) const;

	const T* GetData();

	void Memcpy(const T* data);

	void Fill(const T& value);

	const std::span<T>& GetSpan() const { return datas_; }

	//=========================================================================================
	// operator
	//=========================================================================================

	T& operator[](size_t index);
	const T& operator[](size_t index) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::span<T> datas_;

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

	const UINT GetIndexCount() const;

	const D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// LineIndexDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class LineIndexDimensionBuffer
	: public DimensionBuffer<std::pair<UINT, UINT>> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	const UINT GetIndexCount() const;

	const D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// TriangleIndexDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class TriangleIndexDimensionBuffer
	: public DimensionBuffer<std::tuple<UINT, UINT, UINT>> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	const UINT GetIndexCount() const;

	const D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

private:
};


////////////////////////////////////////////////////////////////////////////////////////////
// DimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
inline void DimensionBuffer<T>::Create(Device* device, uint32_t size) {
	BaseDimensionBuffer::Create(device, size);
	resource_->SetName(L"Dimension Buffer");

	Map();
}

template <class T>
inline void DimensionBuffer<T>::Map() {
	T* target = nullptr;

	// resourceをマッピング
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&target));
	datas_ = { target, size_ };
}

template <class T>
inline void DimensionBuffer<T>::Unmap() {
	if (resource_ != nullptr) {
		resource_->Unmap(0, nullptr);
	}

	datas_ = {};
}

template <class T>
inline T& DimensionBuffer<T>::At(size_t index) {
	Exception::Assert(CheckIndex(index), "Dimension Buffer out of range.");
	return datas_[index];
}

template <class T>
inline const T& DimensionBuffer<T>::At(size_t index) const {
	Exception::Assert(CheckIndex(index), "Dimension Buffer out of range.");
	return datas_[index];
}

template <class T>
inline const T* DimensionBuffer<T>::GetData() {
	return datas_.data();
}

template <class T>
inline void DimensionBuffer<T>::Memcpy(const T* data) {
	std::memcpy(datas_.data(), data, stride_ * size_);
}

template <class T>
inline void DimensionBuffer<T>::Fill(const T& value) {
	std::fill(datas_.begin(), datas_.end(), value);
}

template <class T>
inline T& DimensionBuffer<T>::operator[](size_t index) {
	Exception::Assert(CheckIndex(index), "Dimension Buffer out of range.");
	return datas_[index];
}

template <class T>
inline const T& DimensionBuffer<T>::operator[](size_t index) const {
	Exception::Assert(CheckIndex(index), "Dimension Buffer out of range.");
	return datas_[index];
}

////////////////////////////////////////////////////////////////////////////////////////////
// VertexDimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
inline const D3D12_VERTEX_BUFFER_VIEW VertexDimensionBuffer<T>::GetVertexBufferView() const {
	D3D12_VERTEX_BUFFER_VIEW result = {};
	result.BufferLocation = this->GetGPUVirtualAddress();
	result.SizeInBytes    = static_cast<UINT>(this->stride_ * this->size_);
	result.StrideInBytes  = static_cast<UINT>(this->stride_);

	return result;
}

_DXOBJECT_NAMESPACE_END


