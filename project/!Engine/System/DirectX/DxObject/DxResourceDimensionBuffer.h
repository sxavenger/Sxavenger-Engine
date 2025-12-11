#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxResourceStorage.h"

//* c++
#include <optional>
#include <span>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BaseResourceDimensionBuffer
////////////////////////////////////////////////////////////////////////////////////////////
class BaseResourceDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseResourceDimensionBuffer(size_t stride) : stride_(stride) {}
	virtual ~BaseResourceDimensionBuffer();

	//* getter *//

	ID3D12Resource* GetResource() const;

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	const uint32_t GetSize() const { return size_; }
	const uint32_t& GetDimension() const { return size_; }

	const size_t GetStride() const { return stride_; }

	const size_t GetByteSize() const { return size_ * stride_; }

protected:

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void CreateResource(Device* device, uint32_t size);

	bool CheckIndex(size_t index) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<ResourceBuffer> resource_;

	uint32_t size_       = NULL;
	const size_t stride_ = NULL;

};

////////////////////////////////////////////////////////////////////////////////////////////
// ResourceDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class ResourceDimensionBuffer
	: public BaseResourceDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ResourceDimensionBuffer() : BaseResourceDimensionBuffer(sizeof(T)) {}
	~ResourceDimensionBuffer() override;

	void Create(Device* device, uint32_t size);

	void Map();

	void Unmap();

	//* span option *//

	T& At(size_t index);
	const T& At(size_t index) const;

	void Memcpy(const T* data);

	void Fill(const T& value);

	//* getter *//

	const T* GetData();

	//=========================================================================================
	// operator
	//=========================================================================================

	T& operator[](size_t index);
	const T& operator[](size_t index) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<std::span<T>, ResourceBuffer::kSwapBufferCount> datas_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// DimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
inline ResourceDimensionBuffer<T>::~ResourceDimensionBuffer() {
	Unmap();
}

template <class T>
inline void ResourceDimensionBuffer<T>::Create(Device* device, uint32_t size) {
	BaseResourceDimensionBuffer::CreateResource(device, size);
	resource_->SetName(L"Dimension Buffer");

	Map();
}

template <class T>
inline void ResourceDimensionBuffer<T>::Map() {
	for (size_t i = 0; i < ResourceBuffer::kSwapBufferCount; ++i) {
		T* target = nullptr;
		resource_->Get(i)->Map(0, nullptr, reinterpret_cast<void**>(&target));

		datas_[i] = { target, size_ };
	}
}

template <class T>
inline void ResourceDimensionBuffer<T>::Unmap() {
	if (resource_ == nullptr) {
		return;
	}

	for (size_t i = 0; i < ResourceBuffer::kSwapBufferCount; ++i) {
		resource_->Get(i)->Unmap(0, nullptr);
		datas_[i] = {};
	}
}

template <class T>
inline T& ResourceDimensionBuffer<T>::At(size_t index) {
	Exception::Assert(CheckIndex(index), "Dimension Buffer out of range.");
	return datas_[ResourceStorage::GetInstance()->GetBackBufferIndex()][index];
}

template <class T>
inline const T& ResourceDimensionBuffer<T>::At(size_t index) const {
	Exception::Assert(CheckIndex(index), "Dimension Buffer out of range.");
	return datas_[ResourceStorage::GetInstance()->GetBackBufferIndex()][index];
}

template<class T>
inline void ResourceDimensionBuffer<T>::Memcpy(const T* data) {
	std::memcpy(datas_[ResourceStorage::GetInstance()->GetBackBufferIndex()].data(), data, BaseResourceDimensionBuffer::GetByteSize());
}

template<class T>
inline void ResourceDimensionBuffer<T>::Fill(const T& value) {
	std::fill(datas_[ResourceStorage::GetInstance()->GetBackBufferIndex()].begin(), datas_[ResourceStorage::GetInstance()->GetBackBufferIndex()].end(), value);
}

template<class T>
inline const T* ResourceDimensionBuffer<T>::GetData() {
	return datas_[ResourceStorage::GetInstance()->GetBackBufferIndex()].data();
}

template<class T>
inline T& ResourceDimensionBuffer<T>::operator[](size_t index) {
	Exception::Assert(CheckIndex(index), "Dimension Buffer out of range.");
	return datas_[ResourceStorage::GetInstance()->GetBackBufferIndex()][index];
}

template <class T>
inline const T& ResourceDimensionBuffer<T>::operator[](size_t index) const {
	Exception::Assert(CheckIndex(index), "Dimension Buffer out of range.");
	return datas_[ResourceStorage::GetInstance()->GetBackBufferIndex()][index];
}

DXOBJECT_NAMESPACE_END
