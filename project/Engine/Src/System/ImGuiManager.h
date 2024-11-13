#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// imgui
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

// c++
#include <cstdint>
#include <cassert>

// DxObject
#include <Engine/System/DxObject/DxObjectCommon.h>

// Geometry
#include <Lib/Geometry/Vector4.h>

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
	ImGuiManager() = default;

	//! @brief デストラクタ
	~ImGuiManager() = default;

	//! @brief 初期化処理
	void Init(WinApp* winApp, DirectXCommon* dxCommon);

	//! @brief 終了処理
	void Term();

	//! @brief フレーム開始処理
	void Begin();

	//! @brief フレーム終了処理
	void EndDrawCall();

	static ImVec4 ToImVec4(const Color4i& color);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	ID3D12GraphicsCommandList* commandList_ = nullptr;


	DxObject::Descriptor descriptor_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitImGuiStyle();
	void SettingImGui();

};