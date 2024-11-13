#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObjectCommon
#include "DxObjectCommon.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Devices;
class Fence;

////////////////////////////////////////////////////////////////////////////////////////////
// Command class
////////////////////////////////////////////////////////////////////////////////////////////
class Command {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Command() = default;

	//! @brief コンストラクタ
	//! 
	//! @param[in] devices
	Command(Devices* device) { Init(device); }

	//! @brief デストラクタ
	~Command() { Term(); }

	//! @brief 初期化
	//! 
	//! @param[in] devices
	void Init(Devices* device);

	//! @brief 終了処理
	void Term();

	//! @brief commandListをクローズし, Queueで実行
	void Close();

	//! @brief commandAllocatorのすべてをリセット, commandListをリセット
	void ResetAllAllocator();

	//! @brief backAllocatorの更新 & commandListをbackAllocatorにリセット
	void ResetBackAllocatorAndTranstion();

	//void ResetSingleAllocator();
	//!< testで消してる
	
	void ResetBackAllocator();

	//* Getter *//

	//! @brief コマンドリストを取得
	auto GetCommandList() const { return commandList_.Get(); }

	//! @brief コマンドキューを取得
	ID3D12CommandQueue* const GetCommandQueue() const { return commandQueue_.Get(); }

	//! @brief Signalを実行
	void Signal(Fence* fences);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const uint32_t kAllocatorCount_ = 2;

	ComPtr<ID3D12CommandQueue>         commandQueue_;
	ComPtr<ID3D12CommandAllocator>     commandAllocator_[kAllocatorCount_];
	ComPtr<ID3D12GraphicsCommandList6> commandList_;

	uint32_t backAllocatorIndex_ = 0;

	/*
		backAllocator  -> commandListを積んでるAllocator
		frontAllocator -> 実行中のallocator
	*/

};

_DXOBJECT_NAMESPACE_END