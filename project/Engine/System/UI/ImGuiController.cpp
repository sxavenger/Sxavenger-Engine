#include "ImGuiController.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//=========================================================================================
// static variables
//=========================================================================================

const std::filesystem::path ImGuiController::kImGuiLayoutFilepath_ = "imgui.ini";

////////////////////////////////////////////////////////////////////////////////////////////
// ImGuiController class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ImGuiController::Init(Window* mainWindow) {
	// handleの取得
	//descriptorSRV_ = SxavengerSystem::GetDescriptor(kDescriptor_SRV);

	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	SettingImGui();
	ImGui::StyleColorsDark();
	SetImGuiStyle();

	ImGui_ImplWin32_Init(mainWindow->GetHwnd());
	/*ImGui_ImplDX12_Init(
		SxavengerSystem::GetDxDevice()->GetDevice(),
		DxObject::SwapChain::GetBufferCount(),
		DxObject::kScreenFormat,
		SxavengerSystem::GetDxDescriptorHeaps()->GetDescriptorHeap(kDescriptor_CBV_SRV_UAV),
		descriptorSRV_.GetCPUHandle(),
		descriptorSRV_.GetGPUHandle()
	);*/

	ImGui_ImplDX12_InitInfo info = {};
	info.Device            = SxavengerSystem::GetDxDevice()->GetDevice();
	info.CommandQueue      = SxavengerSystem::GetMainThreadContext()->GetCommandQueue();
	info.NumFramesInFlight = DxObject::SwapChain::GetBufferCount();
	info.RTVFormat         = DxObject::kScreenFormat;
	info.DSVFormat         = DxObject::kDefaultDepthFormat;
	info.SrvDescriptorHeap = SxavengerSystem::GetDxDescriptorHeaps()->GetDescriptorHeap(kDescriptor_CBV_SRV_UAV);

	info.UserData = &descriptorsSRV_;

	info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE* gpuHandle) {
		// descriptorの追加
		auto& descriptors = *static_cast<std::list<DxObject::Descriptor>*>(info->UserData);
		descriptors.emplace_back(SxavengerSystem::GetDxDescriptorHeaps()->GetDescriptor(kDescriptor_SRV));

		// handleの取得
		auto& descriptor = descriptors.back();
		*cpuHandle = descriptor.GetCPUHandle();
		*gpuHandle = descriptor.GetGPUHandle();
	};

	info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) {
		gpuHandle; //!< unused

		// descriptor listの取得
		auto& descriptors = *static_cast<std::list<DxObject::Descriptor>*>(info->UserData);

		// descriptorの削除
		descriptors.remove_if([&](DxObject::Descriptor& descriptor) {
			if (descriptor.GetCPUHandle().ptr == cpuHandle.ptr) {
				descriptor.Delete();
				return true;
			}

			return false;
		});
	};

	ImGui_ImplDX12_Init(&info);

	// iniの読み込み
	std::string filepath = kImGuiLayoutFilepath_.generic_string();
	ImGui::LoadIniSettingsFromDisk(filepath.c_str());
}

void ImGuiController::Term() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//descriptorSRV_.Delete();
}

void ImGuiController::BeginFrame() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();
}

void ImGuiController::EndFrame() {
	ImGui::Render();
}

void ImGuiController::Render(DirectXThreadContext* context) {
#ifdef _DEBUG
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), context->GetCommandList());
#endif // _DEBUG
}

void ImGuiController::OutputLayout() {
	std::string filepath = kImGuiLayoutFilepath_.generic_string();
	ImGui::SaveIniSettingsToDisk(filepath.c_str());
}

ImVec4 ImGuiController::ToImVec4(const Color4i& color) {
	return { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
}

void ImGuiController::SetImGuiStyle() {

	// imguiデザイン設定
	ImGuiStyle& style = ImGui::GetStyle();
	/* size */
	// main
	style.FramePadding.y = 2;
	style.ItemSpacing.y  = 4;
	style.GrabMinSize    = 8;

	// rounding
	style.WindowRounding    = 3;
	style.FrameRounding     = 2;
	style.GrabRounding      = 2;
	style.ScrollbarRounding = 0;
	style.TabRounding       = 2;

	// docking
	style.DockingSeparatorSize = 1;

	/* borderSize */
	style.FrameBorderSize  = 0.0f;
	style.WindowBorderSize = 0.0f;

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
	style.Colors[ImGuiCol_Tab]                       = ToImVec4({ 6, 6, 6, 255 });
	style.Colors[ImGuiCol_TabHovered]                = ToImVec4({ 75, 75, 75, 255 });
	style.Colors[ImGuiCol_TabActive]                 = ToImVec4({ 32, 32, 32, 255 });
	style.Colors[ImGuiCol_TabUnfocused]              = ToImVec4({ 6, 6, 6, 255 });
	style.Colors[ImGuiCol_TabUnfocusedActive]        = ToImVec4({ 15, 15, 15, 255 });
	style.Colors[ImGuiCol_TabSelectedOverline]       = ToImVec4({ 51, 51, 51, 255 });
	style.Colors[ImGuiCol_TabDimmedSelectedOverline] = ToImVec4({ 15, 15, 15, 0 });

	// plot
	style.Colors[ImGuiCol_PlotLines]     = ToImVec4({ 152, 152, 152, 255 });
	style.Colors[ImGuiCol_PlotHistogram] = ToImVec4({ 48, 48, 48, 255 });


	// docking
	style.Colors[ImGuiCol_DockingPreview] = ToImVec4({ 125, 125, 125, 200 });

}

void ImGuiController::SettingImGui() {

	// imguiの設定
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// imgui dockingブランチを参照...
	
	{ //!< fontの変更
		std::string filepath = "packages/font/FiraMono-Regular.ttf";
		io.Fonts->AddFontFromFileTTF(filepath.c_str(), 14.0f);
	}

	{ //!< imguiの書き込み, 読み込みを手動に変更
		io.IniFilename = NULL;
	}

}
