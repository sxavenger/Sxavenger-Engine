#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDimensionBuffer.h"
#include "DxCommandContext.h"

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

	UnorderedDimensionBuffer() : BaseDimensionBuffer(sizeof(T)) {}
	~UnorderedDimensionBuffer() { Release(); }

	void Create(Device* device, uint32_t size);

	void Release();

	void Barrier(CommandContext* context);

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

	// resourceの生成
	resource_ = CreateBufferResource(	
		device->GetDevice(),
		D3D12_HEAP_TYPE_DEFAULT,
		BaseDimensionBuffer::GetByteSize(),
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	address_ = resource_->GetGPUVirtualAddress();
}

template<class T>
inline void UnorderedDimensionBuffer<T>::Release() {
	if (resource_ != nullptr) {
		resource_.Reset();
		address_ = std::nullopt;
	}

	size_ = NULL;
}

template <class T>
inline void UnorderedDimensionBuffer<T>::Barrier(CommandContext* context) {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = GetResource();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

////////////////////////////////////////////////////////////////////////////////////////////
// VertexUnorderedDimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
inline const D3D12_VERTEX_BUFFER_VIEW VertexUnorderedDimensionBuffer<T>::GetVertexBufferView() const {
	D3D12_VERTEX_BUFFER_VIEW result = {};
	result.BufferLocation = this->GetGPUVirtualAddress();
	result.SizeInBytes    = static_cast<UINT>(this->stride_ * this->size_);
	result.StrideInBytes  = static_cast<UINT>(this->stride_);

	return result;
}


_DXOBJECT_NAMESPACE_END
