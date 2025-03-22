#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include <ImGuizmo.h>
#include <imnodes.h>

//* engine
#include <Engine/System/Window/Window.h>
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>

//* lib
#include <Lib/Geometry/Vector4.h>

//* c++
#include <filesystem>
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// ImGuiController class
////////////////////////////////////////////////////////////////////////////////////////////
class ImGuiController {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ImGuiController()  = default;
	~ImGuiController() { Term(); }

	void Init(Window* main);

	void Term();

	void BeginFrame();

	void EndFrame();

	void Render(DirectXThreadContext* context);

	void OutputLayout();

	//* convert *//

	static ImVec4 ToImVec4(const Color4i& color);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::list<DxObject::Descriptor> descriptorsSRV_;

	static const std::filesystem::path kImGuiLayoutFilepath_;
	static const std::filesystem::path kImGuiSampleLayoutFilepath_;

	bool isInit_ = false;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void SetImGuiStyle();
	void SettingImGui();


};
