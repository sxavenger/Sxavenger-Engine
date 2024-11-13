#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObjectCommon
#include "DxObjectCommon.h"

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class WinApp;

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Devices;
class Command;
class DescriptorHeaps;

////////////////////////////////////////////////////////////////////////////////////////////
// SwapChain class
////////////////////////////////////////////////////////////////////////////////////////////
class SwapChain {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 初期化
	//! 
	//! @param[in] devices         DxObject::Devices
	//! @param[in] command         DxObject::Command
	//! @param[in] descriptorHeaps DxObject::DescriptorHeaps
	//! @param[in] winApp          WinApp
	//! @param[in] clientWidth     クライアント横幅
	//! @param[in] clientHeight    クライアント縦幅
	SwapChain(
		Devices* devices, Command* command, DescriptorHeaps* descriptorHeaps, WinApp* winApp
	);

	//! @brief デストラクタ
	~SwapChain();

	//! @brief 初期化
	//! 
	//! @param[in] devices         DxObject::Devices
	//! @param[in] command         DxObject::Command
	//! @param[in] descriptorHeaps DxObject::DescriptorHeaps
	//! @param[in] winApp          WinApp
	//! @param[in] clientWidth     クライアント横幅
	//! @param[in] clientHeight    クライアント縦幅
	void Init(
		Devices* devices, Command* command, DescriptorHeaps* descriptorHeaps, WinApp* winApp
	);

	//! @brief 終了処理
	void Term();

	//! @brief Presentを実行
	void Present(UINT SyncInterval, UINT Flags);

	void ObtainCurrentBackBufferIndex();

	//* getter *//

	//! @brief スワップチェインを取得
	//! 
	//! @return スワップチェインを返却
	IDXGISwapChain4* GetSwapChain() const { return swapChain_.Get(); }

	//! @brief トランジションバリアを取得
	//! 
	//! @param[in] backBufferIndex
	//! @param[in] stateBefore
	//! @param[in] stateAfter
	//! 
	//! @return トランジションバリアを返却
	D3D12_RESOURCE_BARRIER* GetBackBufferTransitionBarrier(
		D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter
	);

	static const uint32_t GetBufferCount() { return kBufferCount_; }

	UINT GetCurrentBackBufferIndex() const { return currentBackBufferIndex_; }

	//! @brief handleCPU_RTVを取得
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandleCPU() const { return descriptorRTV_[currentBackBufferIndex_].GetCPUHandle(); }

	ID3D12Resource* const GetBackBufferResource() const { return resource_[currentBackBufferIndex_].Get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ComPtr<IDXGISwapChain4> swapChain_;

	//* buffers *//

	static const UINT      kBufferCount_ = 2;
	ComPtr<ID3D12Resource> resource_[kBufferCount_];
	DxObject::Descriptor   descriptorRTV_[kBufferCount_];

	UINT                   currentBackBufferIndex_;

	//* barrier *//

	D3D12_RESOURCE_BARRIER barrier_; 
};

_DXOBJECT_NAMESPACE_END