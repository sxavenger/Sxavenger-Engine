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

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::span<T> mappedDatas_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Map();

};

////////////////////////////////////////////////////////////////////////////////////////////
// VectorDimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
inline void VectorDimensionBuffer<T>::Resize(Device* device, uint32_t size) {
	// 引数の保存
	size_ = size;

	if (size_ > capacity_) {
		// 一時的に保存
		std::vector<T> data(mappedDatas_.begin(), mappedDatas_.end());

		// resourceの生成
		BaseVectorDimensionBuffer::Create(device, size_);
		Map();

		std::copy(data.begin(), data.begin(), mappedDatas_.begin());
	}
}

template<class T>
inline void VectorDimensionBuffer<T>::Reserve(Device* device, uint32_t capacity) {
	// 引数の保存
	capacity_ = capacity;
	// resourceの生成
	BaseVectorDimensionBuffer::Create(device, capacity_);
	Map();
}

template<class T>
inline void VectorDimensionBuffer<T>::Map() {
	T* mappingTarget = nullptr;

	// resourceをマッピング
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappingTarget));
	mappedDatas_ = { mappingTarget, size_ };
}

_DXOBJECT_NAMESPACE_END
