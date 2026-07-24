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
	friend struct ReferenceDescriptor;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Handle structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 1つのディスクリプタのindexとCPU/GPUハンドルを保持する構造体
	struct Handle {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor / destructor *//

		Handle(DescriptorType type) : type(type) {}

		//* getter *//

		//! @brief CPUディスクリプタハンドルを取得する
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle() const;

		//! @brief GPUディスクリプタハンドルを取得する
		const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const;

		//! @brief ディスクリプタのindexを取得する
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

////////////////////////////////////////////////////////////////////////////////////////////
// ReferenceDescriptor structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Descriptorの参照を保管するクラス.
struct ReferenceDescriptor {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ReferenceDescriptor()  = default;
	~ReferenceDescriptor() = default;

	//* descriptor option *//

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle() const;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle() const;

	uint32_t GetIndex() const;

	bool HasHandle() const { return handle_.has_value(); }

	//* operator [move] *//

	ReferenceDescriptor(ReferenceDescriptor&&) noexcept            = default;
	ReferenceDescriptor& operator=(ReferenceDescriptor&&) noexcept = default;

	//* operator [copy] *//

	ReferenceDescriptor(const ReferenceDescriptor&) noexcept            = default;
	ReferenceDescriptor& operator=(const ReferenceDescriptor&) noexcept = default;

	//* operator [conversion] *//

	ReferenceDescriptor(const Descriptor& descriptor);
	ReferenceDescriptor& operator=(const Descriptor& descriptor);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	std::optional<Descriptor::Handle> handle_ = std::nullopt;

};

DXOBJECT_NAMESPACE_END
