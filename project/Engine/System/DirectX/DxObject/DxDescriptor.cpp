#include "DxDescriptor.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxDescriptorHeaps.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Descriptor sturcture methods
////////////////////////////////////////////////////////////////////////////////////////////

void Descriptor::Delete() {
	if (descriptorPool_ != nullptr) {
		descriptorPool_->DeleteDescriptor(*this);
	}

	Reset();
}

const D3D12_CPU_DESCRIPTOR_HANDLE& Descriptor::GetCPUHandle() const {
	Assert(type_.has_value(), "descriptor type is nullopt.");
	return handles_.first;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& Descriptor::GetGPUHandle() const {
	Assert(type_.has_value(),           "descriptor type is nullopt.");
	Assert(handles_.second.has_value(), "descriptor type not having a GPUHandle.");
	return handles_.second.value();
}

const uint32_t Descriptor::GetIndex() const {
	Assert(type_.has_value(), "descriptor type is nullopt.");
	return index_;
}

void Descriptor::Reset() {
	type_  = std::nullopt;
	index_ = {};

	handles_.first  = {};
	handles_.second = std::nullopt;

	descriptorPool_ = nullptr;
}
