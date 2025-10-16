#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxDimensionBuffer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ConstantBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class ConstantBuffer
	: public BaseDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ConstantBuffer() : BaseDimensionBuffer(sizeof(T)) {}
	~ConstantBuffer() override { Unmap(); }

	void Create(Device* device);

	void Map();

	void Unmap();

	T& At();
	const T& At() const;

	const T* GetData();

	void Fill(const T& value);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	T* data_ = nullptr;

};

////////////////////////////////////////////////////////////////////////////////////////////
// ConstantBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void ConstantBuffer<T>::Create(Device* device) {
	BaseDimensionBuffer::Create(device, 1);
	resource_->SetName(L"Constant Buffer");

	Map();
}

template <typename T>
inline void ConstantBuffer<T>::Map() {
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
}

template <typename T>
inline void ConstantBuffer<T>::Unmap() {
	if (resource_ != nullptr) {
		resource_->Unmap(0, nullptr);
	}

	data_ = nullptr;
}

template <typename T>
inline T& ConstantBuffer<T>::At() {
	Exception::Assert(data_ != nullptr, "Constant Buffer is not mapped.");
	return *data_;
}

template <typename T>
inline const T& ConstantBuffer<T>::At() const {
	Exception::Assert(data_ != nullptr, "Constant Buffer is not mapped.");
	return *data_;
}

template <typename T>
inline const T* ConstantBuffer<T>::GetData() {
	Exception::Assert(data_ != nullptr, "Constant Buffer is not mapped.");
	return data_;
}

template <typename T>
inline void ConstantBuffer<T>::Fill(const T& value) {
	Exception::Assert(data_ != nullptr, "Constant Buffer is not mapped.");
	std::memcpy(data_, &value, stride_ * size_);
}

_DXOBJECT_NAMESPACE_END
