#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDimensionBuffer.h"
#include "DxCommandContext.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// UnorderedDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief [RWStructuredBuffer] 1次元Bufferクラス.
template <typename T>
class UnorderedDimensionBuffer
	: public BaseDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UnorderedDimensionBuffer() : BaseDimensionBuffer(sizeof(T)) {}
	~UnorderedDimensionBuffer() override {}

	void Create(DxObject::Device* device, uint32_t size);

	//* unordered option *//

	void Barrier(DxObject::CommandContext* context) const;

	void TransitionUnordered(DxObject::CommandContext* context);

	void TransitionDefault(DxObject::CommandContext* context);

};

////////////////////////////////////////////////////////////////////////////////////////////
// UnorderedByteAddressBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief [RWByteAddressBuffer] 1次元Bufferクラス.
class UnorderedByteAddressBuffer
	: public BaseDimensionBuffer {
public:

	UnorderedByteAddressBuffer() : BaseDimensionBuffer(1) {}
	~UnorderedByteAddressBuffer() override {}

	void Create(DxObject::Device* device, uint32_t byte);

	//* unordered option *//

	void Barrier(DxObject::CommandContext* context) const;

	void TransitionUnordered(DxObject::CommandContext* context);

	void TransitionDefault(DxObject::CommandContext* context);

};

////////////////////////////////////////////////////////////////////////////////////////////
// UnorderedDimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void UnorderedDimensionBuffer<T>::Create(DxObject::Device* device, uint32_t size) {
	BaseDimensionBuffer::CreateBuffer(device, size, Category::Default);
	resource_.SetName(L"Unordered Dimension Buffer");
}

template <typename T>
inline void UnorderedDimensionBuffer<T>::Barrier(DxObject::CommandContext* context) const {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = GetResource();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

template <typename T>
inline void UnorderedDimensionBuffer<T>::TransitionUnordered(DxObject::CommandContext* context) {
	resource_.Transition(context, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

template<typename T>
inline void UnorderedDimensionBuffer<T>::TransitionDefault(DxObject::CommandContext* context) {
	resource_.Transition(context, BaseDimensionBuffer::GetDefaultState(Category::Default));
}


DXOBJECT_NAMESPACE_END
