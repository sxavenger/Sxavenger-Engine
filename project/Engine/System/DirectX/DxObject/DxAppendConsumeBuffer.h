#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxDescriptor.h"
#include "DxResource.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>

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
	virtual ~BaseAppendConsumeBuffer() { Reset(); }

	//* option *//

	void Reset();

	//* getter *//

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV() const;

	const uint32_t GetSize() const { return size_; }

	const size_t GetStride() const { return stride_; }

	const size_t GetByteSize() const { return size_ * stride_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================
	
	//* DirectX12 *//

	DxObject::Resource resource_;
	DxObject::Resource counter_;
	DxObject::Descriptor descriptorUAV_;

	//* parameter *//

	uint32_t size_       = NULL;
	const size_t stride_ = NULL;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void CreateBuffer(DxObject::Device* device, DxObject::Descriptor&& descriptor, uint32_t size);

};

////////////////////////////////////////////////////////////////////////////////////////////
// AppendConsumeBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class AppendConsumeBuffer
	: public BaseAppendConsumeBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AppendConsumeBuffer() : BaseAppendConsumeBuffer(sizeof(T)) {}
	~AppendConsumeBuffer() override = default;

	void Create(DxObject::Device* device, DxObject::Descriptor&& descriptor, uint32_t size);

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// AppendConsumeBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void AppendConsumeBuffer<T>::Create(DxObject::Device* device, DxObject::Descriptor&& descriptor, uint32_t size) {
	BaseAppendConsumeBuffer::CreateBuffer(device, std::move(descriptor), size);
	resource_.SetName(L"Append Consume Buffer");
	counter_.SetName(L"Append Consume Counter");
}

DXOBJECT_NAMESPACE_END
