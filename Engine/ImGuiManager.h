#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// imgui
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

#include <DxObjectMethod.h>

// c++
#include <cstdint>
#include <cassert>

// Geometry
#include <Vector4.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class WinApp;
class DirectXCommon;

////////////////////////////////////////////////////////////////////////////////////////////
// ImGuiManager class
////////////////////////////////////////////////////////////////////////////////////////////
class ImGuiManager {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	ImGuiManager();

	//! @brief デストラクタ
	~ImGuiManager();

	//! @brief 初期化処理
	void Init(WinApp* winApp, DirectXCommon* dxCommon);

	//! @brief 終了処理
	void Term();

	//! @brief フレーム開始処理
	void Begin();

	//! @brief フレーム終了処理
	void End();

	//! @brief インスタンスを確保
	//! 
	//! @return インスタンスを返却
	static ImGuiManager* GetInstaince();

	static ImVec4 ToImVec4(const Color4i& color);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	DirectXCommon* dxCommon_;

	ID3D12DescriptorHeap* descriptorHeap_SRV_;

	DxObject::Descriptor descriptor_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitImGuiStyle();
	void SettingImGui();

};