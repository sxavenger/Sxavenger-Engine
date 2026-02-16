#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxCommandContext.h"
#include "DxDimensionBuffer.h"
#include "DxUnorderedDimensionBuffer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ReadbackDimensionBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class ReadbackDimensionBuffer
	: public BaseDimensionBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ReadbackDimensionBuffer() : BaseDimensionBuffer(sizeof(T)) {}
	~ReadbackDimensionBuffer() override { Unmap(); }

	void Capacity(
		DxObject::Device* device,
		size_t size
	);

	void Readback(
		DxObject::Device* device, DxObject::CommandContext* context,
		UnorderedDimensionBuffer<T>* source
	);

	//* getter *//

	const std::span<T>& GetSpan() const { return data_; }

	//* access option *//

	const T& At(size_t index) const;

	const T& operator[](size_t index) const { return At(index); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::span<T> data_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Map();

	void Unmap();

};

////////////////////////////////////////////////////////////////////////////////////////////
// ReadbackDimensionBuffer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline void ReadbackDimensionBuffer<T>::Capacity(
	DxObject::Device* device,
	size_t size) {

	if (GetSize() != size) { //!< サイズが異なる場合は再作成
		BaseDimensionBuffer::CreateBuffer(device, static_cast<uint32_t>(size), Category::Readback);
		resource_.SetName(L"Readback Dimension Buffer");
	}

	Map();
}

template <typename T>
inline void ReadbackDimensionBuffer<T>::Readback(
	DxObject::Device* device, DxObject::CommandContext* context,
	UnorderedDimensionBuffer<T>* source) {

	//!< resourceの作成
	if (GetSize() != source->GetSize()) { //!< サイズが異なる場合は再作成
		BaseDimensionBuffer::CreateBuffer(device, source->GetSize(), Category::Readback);
		resource_.SetName(L"Readback Dimension Buffer");
	}

	//!< コピー
	D3D12_RESOURCE_STATES state = source->Get().GetCurrentState();

	source->Get().Transition(context, D3D12_RESOURCE_STATE_COPY_SOURCE);

	context->GetCommandList()->CopyResource(
		resource_.Get(),
		source->GetResource()
	);

	source->Get().Transition(context, state);

	Map();
}

template <typename T>
inline const T& ReadbackDimensionBuffer<T>::At(size_t index) const {
	SXAVENGER_ENGINE StreamLogger::AssertA(index < size_, "Readback Dimension Buffer out of range.");
	return data_[index];
}

template <typename T>
inline void ReadbackDimensionBuffer<T>::Map() {
	T* ptr = nullptr;

	//!< resourceをマッピング
	resource_.Map(reinterpret_cast<void**>(&ptr));
	data_ = std::span<T>(ptr, size_);
}

template <typename T>
inline void ReadbackDimensionBuffer<T>::Unmap() {
	if (resource_ != nullptr) {
		resource_.Unmap();
	}

	data_ = {};
}


DXOBJECT_NAMESPACE_END
