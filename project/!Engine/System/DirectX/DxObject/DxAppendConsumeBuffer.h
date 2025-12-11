#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxDescriptorHeaps.h"
#include "DxDescriptor.h"

//* c++
#include <optional>
#include <span>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BaseAppendConsumeBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Append/Consume Bufferの基底クラス.
class BaseAppendConsumeBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseAppendConsumeBuffer(size_t stride) : stride_(stride) {}
	virtual ~BaseAppendConsumeBuffer() { Release(); }

	void Release();

	//* getter *//

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetAppendCousumeGPUHandleUAV() const;

	const D3D12_GPU_VIRTUAL_ADDRESS& GetCounterGPUVirtualAddress() const;

	const uint32_t GetSize() const { return size_; }

	const size_t GetStride() const { return stride_; }

	const size_t GetByteSize() const { return size_ * stride_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12Resource> resource_;
	ComPtr<ID3D12Resource> counter_;
	Descriptor descriptorUAV_;

	std::optional<D3D12_GPU_VIRTUAL_ADDRESS> counterAddress_ = std::nullopt;

	//* paraemter *//

	uint32_t size_       = NULL;
	const size_t stride_ = NULL;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* helper methods *//

	void Create(Device* device, DescriptorHeaps* descriptorHeaps, uint32_t size, uint32_t counterSize);

};

////////////////////////////////////////////////////////////////////////////////////////////
// AppendConsumeBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Append/Consume Bufferを管理するクラス.
template <class T>
class AppendConsumeBuffer
	: public BaseAppendConsumeBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AppendConsumeBuffer() : BaseAppendConsumeBuffer(sizeof(T)) {}
	~AppendConsumeBuffer() override = default;

	void Create(Device* device, DescriptorHeaps* descriptorHeaps, uint32_t size, uint32_t counterSize = 1);

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// AppendConsumeBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void AppendConsumeBuffer<T>::Create(Device* device, DescriptorHeaps* descriptorHeaps, uint32_t size, uint32_t counterSize) {
	BaseAppendConsumeBuffer::Create(device, descriptorHeaps, size, counterSize);
}

DXOBJECT_NAMESPACE_END
