#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"

//* c++
#include <optional>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class DescriptorPool;

////////////////////////////////////////////////////////////////////////////////////////////
// Descriptor structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Descriptor情報を保管するクラス.
class Descriptor {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// friend
	////////////////////////////////////////////////////////////////////////////////////////////
	friend class DescriptorPool;
	friend class DescriptorHeaps;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Handle structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Handle {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor / destructor *//

		Handle(DescriptorType type) : type(type) {}

		//* getter *//

		const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle() const;

		const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const;

		uint32_t GetIndex() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* type *//

		DescriptorType type;

		//* Handle Index *//

		uint32_t index = NULL;

		//* Descriptor Handle *//

		D3D12_CPU_DESCRIPTOR_HANDLE cpu                = {};
		std::optional<D3D12_GPU_DESCRIPTOR_HANDLE> gpu = std::nullopt;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Descriptor() = default;
	~Descriptor() { Reset(); }

	void Reset();

	//* descriptor option *//

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle() const;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const;

	uint32_t GetIndex() const;

	bool HasHandle() const { return handle_.has_value(); }

	//* operator [move] *//

	Descriptor(Descriptor&&) noexcept;
	Descriptor& operator=(Descriptor&&) noexcept;

	//* operator [copy] (delete) *//

	Descriptor(const Descriptor&)            = delete;
	Descriptor& operator=(const Descriptor&) = delete;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	DescriptorPool* pool_ = nullptr;

	//* parameter *//

	std::optional<Handle> handle_ = std::nullopt;

};

DXOBJECT_NAMESPACE_END
