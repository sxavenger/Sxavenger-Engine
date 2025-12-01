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

	//! @brief Commandの実行/Allocatorの遷移を行う
	void TransitionAllocator() const;

	//! @brief 全てのAllocatorを実行する
	void ExecuteAllAllocators() const;

	//! @brief CommandList上でイベントを開始する
	//! @param[in] name イベント名
	void BeginEvent(const std::wstring& name) const;

	//! @brief CommandList上でイベントを終了する
	void EndEvent() const;

	//! @brief 指定したRenderQueueが対応されているか確認する
	//! @retval true  実行可能
	//! @retval false 実行不可能
	bool IsSupportQueue(RenderQueue type) const;

	//! @brief 指定したRenderQueueを要求する
	//! @throw 指定したRenderQueueが対応されていない場合に例外をスローする
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
