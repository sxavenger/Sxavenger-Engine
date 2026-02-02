#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ui
#include "SxGui.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXWindowContext.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/System/UI/SxImGui.h>

//* external
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

//* c++
#include <filesystem>
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ImGuiController class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ImGuiの管理クラス.
class ImGuiController {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ImGuiController()  = default;
	~ImGuiController() { Term(); }

	void Init(DirectXWindowContext* main);

	void Term();

	void BeginFrame();

	void EndFrame();

	//! @brief ImGuiの描画処理を行う
	void Render(DirectXQueueContext* context);

	//* convert *//

	static ImVec4 ToImVec4(const Color4i& color);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::list<DxObject::Descriptor> descriptorsSRV_;

	bool isInit_ = false;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void SetImGuiStyle();


};

SXAVENGER_ENGINE_NAMESPACE_END
