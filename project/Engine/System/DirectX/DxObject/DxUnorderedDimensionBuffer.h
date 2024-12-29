#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDimensionBuffer.h"

//* c++


////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// UnorderedDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class UnorderedDimensionBuffer
	: public BaseDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UnorderedDimensionBuffer()  = default;
	~UnorderedDimensionBuffer() { Release(); }

	void Create(Device* device, uint32_t size);

	void Release();

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// VertexUnorderedDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class VertexUnorderedDimensionBuffer
	: public UnorderedDimensionBuffer<T> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	const D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// UnorderedDimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
inline void UnorderedDimensionBuffer<T>::Create(Device* device, uint32_t size) {

	// 引数の保存
	size_   = size;
	stride_ = static_cast<uint32_t>(sizeof(T));

	// resourceの生成
	resource_ = CreateBufferResource(
		device->GetDevice(),
		D3D12_HEAP_TYPE_DEFAULT,
		size_ * stride_,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON
	);

	address_ = resource_->GetGPUVirtualAddress();
}

template<class T>
inline void UnorderedDimensionBuffer<T>::Release() {
	if (resource_ != nullptr) {
		resource_.Reset();
	}

	size_   = NULL;
	stride_ = NULL;

	address_ = std::nullopt;
}

////////////////////////////////////////////////////////////////////////////////////////////
// VertexUnorderedDimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
inline const D3D12_VERTEX_BUFFER_VIEW VertexUnorderedDimensionBuffer<T>::GetVertexBufferView() const {
	D3D12_VERTEX_BUFFER_VIEW result = {};
	result.BufferLocation = this->GetGPUVirtualAddress();
	result.SizeInBytes    = this->stride_ * this->size_;
	result.StrideInBytes  = this->stride_;

	return result;
}


_DXOBJECT_NAMESPACE_END
