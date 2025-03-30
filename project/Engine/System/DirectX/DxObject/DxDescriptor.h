#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class DescriptorPool;

////////////////////////////////////////////////////////////////////////////////////////////
// Descriptor sturcture
////////////////////////////////////////////////////////////////////////////////////////////
class Descriptor {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using DescriptorHandles = std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, std::optional<D3D12_GPU_DESCRIPTOR_HANDLE>>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// friend
	////////////////////////////////////////////////////////////////////////////////////////////
	friend class DescriptorPool;
	friend class DescriptorHeaps;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Descriptor()  = default;
	~Descriptor() = default;

	void Delete();

	//* getter *//

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle() const;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const;

	const uint32_t GetIndex() const;

	//* operator *//

	Descriptor(Descriptor&&) = default;
	Descriptor& operator=(Descriptor&&) = default;

	Descriptor(const Descriptor&) = delete;
	Descriptor& operator=(const Descriptor&) = delete;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	DescriptorPool* descriptorPool_ = nullptr;

	//* parameter *//

	std::optional<DescriptorType> type_ = std::nullopt;

	uint32_t          index_;
	DescriptorHandles handles_;

	//=========================================================================================
	// private variables
	//=========================================================================================

	void Reset();
};

_DXOBJECT_NAMESPACE_END
