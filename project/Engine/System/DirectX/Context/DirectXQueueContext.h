#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "../DxObject/DxCommandContext.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXQueueContext class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief DirectXのCommand関係を管理するクラス
class DirectXQueueContext {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RenderQueue enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class RenderQueue : uint8_t {
		None,    //!< CommandQueueを作成しない
		Copy,    //!< D3D12_COMMAND_LIST_TYPE_COPY
		Compute, //!< D3D12_COMMAND_LIST_TYPE_COMPUTE
		Direct   //!< D3D12_COMMAND_LIST_TYPE_DIRECT
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DirectXQueueContext()  = default;
	~DirectXQueueContext() = default;

	void Init(uint32_t allocator, RenderQueue type);

	void SetName(const std::wstring& name) const;

	//* context option *//

	void TransitionAllocator() const;

	void ExecuteAllAllocators() const;

	bool IsSupportQueue(RenderQueue type) const;

	void RequestQueue(RenderQueue type) const;

	DxObject::CommandContext* GetDxCommand() const;

	ID3D12GraphicsCommandList6* GetCommandList() const;

	ID3D12CommandQueue* GetCommandQueue() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DXOBJECT *//

	std::unique_ptr<DxObject::CommandContext> context_;

	//* parameter *//

	RenderQueue type_ = RenderQueue::None;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper methods *//

	static D3D12_COMMAND_LIST_TYPE GetCommandListType(RenderQueue type);

	void SetDescriptorHeap() const;

};
