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
template <typename T>
class UnorderedDimensionBuffer
	: public BaseDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================
	
	UnorderedDimensionBuffer() : BaseDimensionBuffer(sizeof(T)) {}
	~UnorderedDimensionBuffer() override {}

	//* option *//

	void Create(DxObject::Device* device, uint32_t size);

	void Barrier(DxObject::CommandContext* context) const;

	void TransitionBeginUnordered(DxObject::CommandContext* context);

	void TransitionEndUnordered(DxObject::CommandContext* context);

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
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = GetResource();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

template <typename T>
inline void UnorderedDimensionBuffer<T>::TransitionBeginUnordered(DxObject::CommandContext* context) {
	resource_.Transition(context, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

template <typename T>
inline void UnorderedDimensionBuffer<T>::TransitionEndUnordered(DxObject::CommandContext* context) {
	resource_.Transition(context, D3D12_RESOURCE_STATE_COMMON);
}

DXOBJECT_NAMESPACE_END
