#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxCommandContext.h"
#include "DxUnorderedDimensionBuffer.h"

//* c++
#include <optional>
#include <span>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ReadbackDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class ReadbackDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ReadbackDimensionBuffer() = default;
	~ReadbackDimensionBuffer() { Release(); }

	void Capacity(Device* device, uint32_t capacity);

	void Resize(uint32_t size);

	void Release();

	//* accessor option *//

	const T& At(size_t index) const {
		Exception::Assert(index < size_, "Readback Dimension Buffer out of range.");
		return mappedDatas_[index];
	}

	const std::span<T>& GetSpan() const {
		return mappedDatas_;
	}

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const uint32_t GetSize() const { return size_; }

	const size_t GetStride() const { return stride_; }

	const size_t GetByteSize() const { return size_ * stride_; }

	//* static methods *//

	static void Readback(Device* device, CommandContext* context, UnorderedDimensionBuffer<T>* src, ReadbackDimensionBuffer<T>* dst);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12Resource> resource_;

	//* parameter *//

	uint32_t capacity_          = NULL;
	uint32_t size_              = NULL;
	static const size_t stride_ = sizeof(T);

	//* mapped data *//

	std::span<T> mappedDatas_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// ReadbackDimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
inline void ReadbackDimensionBuffer<T>::Capacity(Device* device, uint32_t capacity) {
	if (capacity <= capacity_) {
		return; //!< 既に十分な容量がある場合は何もしない
	}

	// capacityの保存
	capacity_ = capacity;

	// resourceの生成
	resource_ = CreateBufferResource(
		device->GetDevice(),
		D3D12_HEAP_TYPE_READBACK,
		capacity_ * stride_,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	resource_->SetName(L"readback dimension buffer");
}

template <class T>
inline void ReadbackDimensionBuffer<T>::Resize(uint32_t size) {
	Exception::Assert(size <= capacity_, "Readback Dimension Buffer size exceeds capacity.");

	// sizeの保存
	size_ = size;

	// resourceのマッピング
	T* mappingTarget = nullptr;
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappingTarget));

	mappedDatas_ = { mappingTarget, size_ };
}

template <class T>
inline void ReadbackDimensionBuffer<T>::Release() {
	size_     = NULL;
	capacity_ = NULL;

	if (resource_ != nullptr) {
		resource_->Unmap(0, nullptr);
		resource_.Reset();
	}

	mappedDatas_ = {};
}

template <class T>
inline void ReadbackDimensionBuffer<T>::Readback(Device* device, CommandContext* context, UnorderedDimensionBuffer<T>* src, ReadbackDimensionBuffer<T>* dst) {

	auto commandList = context->GetCommandList();

	// dstのcapacityを設定
	dst->Capacity(device, src->GetSize());
	dst->Resize(src->GetSize());

	// barrierを設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = src->GetResource();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE;

	commandList->ResourceBarrier(1, &barrier);

	// copyコマンドを発行
	commandList->CopyResource(dst->GetResource(), src->GetResource());

	// barrierを設定
	std::swap(barrier.Transition.StateBefore, barrier.Transition.StateAfter);
	commandList->ResourceBarrier(1, &barrier);
}

_DXOBJECT_NAMESPACE_END
