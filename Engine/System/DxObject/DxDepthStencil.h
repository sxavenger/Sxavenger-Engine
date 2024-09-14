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
class DescriptorHeaps;

////////////////////////////////////////////////////////////////////////////////////////////
// DepthStencil class
////////////////////////////////////////////////////////////////////////////////////////////
class DepthStencil {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DepthStencil() = default;

	//! @brief コンストラクタ
	//! 
	//! @param[in] devices         DxObject::Devices
	//! @param[in] descriptorHeaps DxObject::DescriptorHeaps
	//! @param[in] clientWidth     クライアント領域横幅
	//! @param[in] clientHeight    クライアント領域縦幅
	DepthStencil(
		Devices* devices, DescriptorHeaps* descriptorHeaps, WinApp* winApp
	);

	//! @brief デストラクタ
	~DepthStencil();

	//! @brief 初期化処理
	//! 
	//! @param[in] devices         DxObject::Devices
	//! @param[in] descriptorHeaps DxObject::DescriptorHeaps
	//! @param[in] clientWidth     クライアント領域横幅
	//! @param[in] clientHeight    クライアント領域縦幅
	void Init(
		Devices* devices, DescriptorHeaps* descriptorHeaps, WinApp* winApp
	);

	//! @brief 終了処理
	void Term();

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetHandle() const { return descriptorDSV_.GetCPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ComPtr<ID3D12Resource> resource_;

	DxObject::Descriptor descriptorDSV_;

};

_DXOBJECT_NAMESPACE_END