#include "DxDescriptor.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxDescriptorHeaps.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Handle structure methods
////////////////////////////////////////////////////////////////////////////////////////////

const D3D12_CPU_DESCRIPTOR_HANDLE& Descriptor::Handle::GetCPUHandle() const {
	return cpu;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& Descriptor::Handle::GetGPUHandle() const {
	StreamLogger::AssertA(gpu.has_value(), "handle is not shader visibility.");
	return gpu.value();
}

uint32_t Descriptor::Handle::GetIndex() const {
	return index;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Descriptor structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Descriptor::Reset() {
	if (handle_.has_value()) {
		pool_->DeleteDescriptor(*this);
	}
}

const D3D12_CPU_DESCRIPTOR_HANDLE& Descriptor::GetCPUHandle() const {
	StreamLogger::AssertA(handle_.has_value(), "descriptor is not valid.");
	return handle_->GetCPUHandle();
}

const D3D12_GPU_DESCRIPTOR_HANDLE& Descriptor::GetGPUHandle() const {
	StreamLogger::AssertA(handle_.has_value(), "descriptor is not valid.");
	return handle_->GetGPUHandle();
}

uint32_t Descriptor::GetIndex() const {
	StreamLogger::AssertA(handle_.has_value(), "descriptor is not valid.");
	return handle_->GetIndex();
}

Descriptor::Descriptor(Descriptor&& other) noexcept {
	*this = std::move(other);
}

Descriptor& Descriptor::operator=(Descriptor&& other) noexcept {
	pool_   = std::move(other.pool_);
	handle_ = std::move(other.handle_);

	other.pool_   = nullptr;
	other.handle_ = std::nullopt;

	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////
// ReferenceDescriptor structure
////////////////////////////////////////////////////////////////////////////////////////////

const D3D12_CPU_DESCRIPTOR_HANDLE& ReferenceDescriptor::GetCPUHandle() const {
	StreamLogger::AssertA(handle_.has_value(), "reference descriptor is not valid.");
	return handle_->GetCPUHandle();
}

const D3D12_GPU_DESCRIPTOR_HANDLE& ReferenceDescriptor::GetGPUHandle() const {
	StreamLogger::AssertA(handle_.has_value(), "reference descriptor is not valid.");
	return handle_->GetGPUHandle();
}

uint32_t ReferenceDescriptor::GetIndex() const {
	StreamLogger::AssertA(handle_.has_value(), "reference descriptor is not valid.");
	return handle_->GetIndex();
}

ReferenceDescriptor::ReferenceDescriptor(const Descriptor& descriptor) {
	handle_ = descriptor.handle_;
}

ReferenceDescriptor& ReferenceDescriptor::operator=(const Descriptor& descriptor) {
	handle_ = descriptor.handle_;
	return *this;
}
