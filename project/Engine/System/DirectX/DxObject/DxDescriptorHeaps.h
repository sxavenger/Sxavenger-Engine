#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxDescriptor.h"

//* engine
#include <Engine/System/UI/ISystemDebugGui.h>

//* lib
#include <Lib/CXXAttribute.h>
#include <Lib/Sxl/IndexAllocator.h>

// c++
#include <queue>
#include <memory>
#include <array>
#include <mutex>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorPool class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief DescriptorHeapを管理するクラス.
class DescriptorPool {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DescriptorPool()  = default;
	~DescriptorPool() { Term(); }

	void Init(
		Device* device,
		DescriptorType type, bool shaderVisible,
		uint32_t capacity
	);

	void Term();

	//* descriptor option *//

	NODISCARD Descriptor GetDescriptor();

	void DeleteDescriptor(Descriptor& descriptor);

	//* getter *//

	ID3D12DescriptorHeap* const GetDescriptorHeap() const { return descriptorHeap_.Get(); }

	const uint32_t GetDescriptorCapacity() const { return allocator_.GetCapacity(); }

	const uint32_t GetUsedDescriptorsCount() const { return allocator_.GetUsedCount(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* descriptorHeap *//

	ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	DescriptorType               type_;
	UINT                         descriptorHandleSize_;

	//* descriptorPool *//

	Sxl::IndexAllocator<uint32_t> allocator_;

	//* config *//

	bool shaderVisible_ = false;

	//* thread *//

	std::mutex mutex_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static D3D12_DESCRIPTOR_HEAP_TYPE GetDescriptorHeapType(DescriptorType type);

	void CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type);

	//* helper method *//

	uint32_t GetCurrentDescriptorIndex();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

};

////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorHeaps class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief すべてのDescriptorPoolを管理するクラス.
class DescriptorHeaps
	: public SXAVENGER_ENGINE ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DescriptorHeaps() = default;
	~DescriptorHeaps() { Term(); }

	void Init(Device* device);

	void Term();

	//* descriptor option *//

	NODISCARD Descriptor GetDescriptor(DescriptorType type);

	void DeleteDescriptor(Descriptor& descriptor);

	//* getter *//

	const uint32_t GetDescriptorCapacity(DescriptorType type) const { return pools_.at(type)->GetDescriptorCapacity(); }

	ID3D12DescriptorHeap* const GetDescriptorHeap(DescriptorType type) const { return pools_.at(type)->GetDescriptorHeap(); }

	DescriptorPool* const GetDescriptorPool(DescriptorType type) const { return pools_.at(type).get(); }

	//* imgui option *//

	void SystemDebugGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<std::unique_ptr<DescriptorPool>, DescriptorType::kCountOfDescriptorTypeCount> pools_;

};

DXOBJECT_NAMESPACE_END
