#include "ImGuiManager.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <WinApp.h>
#include <DirectXCommon.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ImGuiManager methods
////////////////////////////////////////////////////////////////////////////////////////////
ImGuiManager::ImGuiManager() {
}

ImGuiManager::~ImGuiManager() {
}

void ImGuiManager::Init(WinApp* winApp, DirectXCommon* dxCommon) {
	assert(winApp != nullptr && dxCommon != nullptr);

	dxCommon_ = dxCommon;

	// SRVの取り出し
	descriptorHeap_SRV_ = dxCommon_->GetDescriptorsObj()->GetDescriptorHeap(DxObject::DescriptorType::SRV);

	// handleの取得
	descriptor_ = dxCommon->GetDescriptorsObj()->GetCurrentDescriptor(DxObject::DescriptorType::SRV);

	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	SettingImGui();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(
		dxCommon_->GetDeviceObj()->GetDevice(),
		DxObject::SwapChain::GetBufferCount(),
		DxObject::defaultFormat,
		descriptorHeap_SRV_,
		descriptor_.GetCPUHandle(),
		descriptor_.GetGPUHandle()
	);

	InitImGuiStyle();

}

void ImGuiManager::Term() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	descriptorHeap_SRV_ = nullptr;
	dxCommon_ = nullptr;
}

void ImGuiManager::Begin() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap_SRV_ };
	dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

void ImGuiManager::End() {
	ImGui::Render();

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList());
}

ImGuiManager* ImGuiManager::GetInstaince() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::InitImGuiStyle() {
	// imguiデザイン設定
	ImGuiStyle& style = ImGui::GetStyle();
	/* size */
	// main
	style.GrabMinSize = 10;

	// rounding
	style.WindowRounding    = 3;
	style.FrameRounding     = 2;
	style.GrabRounding      = 2;
	style.ScrollbarRounding = 0;
	style.TabRounding       = 2;

	// docking
	style.DockingSeparatorSize = 1;

	/* borderSize */
	style.FrameBorderSize = 0.0f;

	/* color */
	// text
	style.Colors[ImGuiCol_Text] = ToImVec4({ 250, 250, 250, 255 });

	// windowBg
	style.Colors[ImGuiCol_WindowBg] = ToImVec4({ 14, 14, 14, 255 });

	// childBg
	style.Colors[ImGuiCol_ChildBg] = ToImVec4({ 5, 5, 5, 255 });

	// border
	style.Colors[ImGuiCol_Border]       = ToImVec4({ 54, 54, 54, 50 });
	style.Colors[ImGuiCol_BorderShadow] = ToImVec4({ 2, 2, 2, 40 });

	// frameBg
	style.Colors[ImGuiCol_FrameBg]        = ToImVec4({ 7, 7, 7, 255 });
	style.Colors[ImGuiCol_FrameBgHovered] = ToImVec4({ 70, 70, 70, 70 });
	style.Colors[ImGuiCol_FrameBgActive]  = ToImVec4({ 100, 100, 100, 80 });

	// titleBg
	style.Colors[ImGuiCol_TitleBgActive]    = ToImVec4({ 20, 20, 20, 255 });
	style.Colors[ImGuiCol_TitleBgCollapsed] = ToImVec4({ 10, 10, 10, 130 });

	// menuBarBg
	style.Colors[ImGuiCol_MenuBarBg] = ToImVec4({ 30, 30, 30, 255 });

	// scrollBar
	style.Colors[ImGuiCol_ScrollbarBg]          = ToImVec4({ 5, 5, 5, 20 });
	style.Colors[ImGuiCol_ScrollbarGrab]        = ToImVec4({ 120, 120, 120, 15 });
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ToImVec4({ 120, 120, 120, 60 });
	style.Colors[ImGuiCol_ScrollbarGrabActive]  = ToImVec4({ 120, 120, 120, 120 });

	// checkMark
	style.Colors[ImGuiCol_CheckMark] = ToImVec4({ 120, 120, 120, 255 });

	// sliderGrab
	style.Colors[ImGuiCol_SliderGrab]       = ToImVec4({ 80, 80, 80, 255 });
	style.Colors[ImGuiCol_SliderGrabActive] = ToImVec4({ 150, 150, 150, 255 });

	// button
	style.Colors[ImGuiCol_Button]        = ToImVec4({ 31, 31, 31, 140 });
	style.Colors[ImGuiCol_ButtonHovered] = ToImVec4({ 50, 50, 50, 140 });
	style.Colors[ImGuiCol_ButtonActive]  = ToImVec4({ 55, 55, 55, 255 });

	// header
	style.Colors[ImGuiCol_Header]        = ToImVec4({ 30, 30, 30, 255 });
	style.Colors[ImGuiCol_HeaderHovered] = ToImVec4({ 52, 52, 52, 255 });
	style.Colors[ImGuiCol_HeaderActive]  = ToImVec4({ 73, 73, 73, 255 });

	// separator
	style.Colors[ImGuiCol_Separator]        = ToImVec4({ 110, 110, 110, 255 });
	style.Colors[ImGuiCol_SeparatorHovered] = ToImVec4({ 110, 110, 110, 0 });
	style.Colors[ImGuiCol_SeparatorActive]  = ToImVec4({ 125, 125, 125, 255 });

	// resizeGrip
	style.Colors[ImGuiCol_ResizeGrip]        = ToImVec4({ 63, 63, 63, 50 });
	style.Colors[ImGuiCol_ResizeGripHovered] = ToImVec4({ 70, 70, 70, 180 });
	style.Colors[ImGuiCol_ResizeGripActive]  = ToImVec4({ 79, 79, 79, 242 });

	// tab
	style.Colors[ImGuiCol_Tab]                = ToImVec4({ 6, 6, 6, 255 });
	style.Colors[ImGuiCol_TabHovered]         = ToImVec4({ 75, 75, 75, 255 });
	style.Colors[ImGuiCol_TabActive]          = ToImVec4({ 32, 32, 32, 255 });
	style.Colors[ImGuiCol_TabUnfocused]       = ToImVec4({ 6, 6, 6, 255 });
	style.Colors[ImGuiCol_TabUnfocusedActive] = ToImVec4({ 15, 15, 15, 255 });

	// docking
	style.Colors[ImGuiCol_DockingPreview] = ToImVec4({ 125, 125, 125, 200 });

}

void ImGuiManager::SettingImGui() {

	// imguiの設定
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	/*io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;*/
	// imgui dockingブランチを参照...

	// fontの変更
	{
		std::string filePath = "Resources/font/FiraMono-Regular.ttf";

		io.Fonts->AddFontFromFileTTF(filePath.c_str(), 14.0f);
	}

}

ImVec4 ImGuiManager::ToImVec4(const Color4i& color) {
	ImVec4 result;
	result = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
	return result;
}
