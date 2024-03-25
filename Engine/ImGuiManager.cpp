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
	descriptorIndex_ = dxCommon_->GetDescriptorsObj()->GetDescriptorCurrentIndex(DxObject::DescriptorType::SRV);

	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(
		dxCommon_->GetDeviceObj()->GetDevice(),
		dxCommon_->GetSwapChainObj()->GetBufferCount(),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, //!< RTV -> desc.Format
		descriptorHeap_SRV_,
		dxCommon_->GetDescriptorsObj()->GetCPUDescriptorHandle(DxObject::DescriptorType::SRV, descriptorIndex_),
		dxCommon_->GetDescriptorsObj()->GetGPUDescriptorHandle(DxObject::DescriptorType::SRV, descriptorIndex_)
	);

	// imguiの設定
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	io.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;
	// imgui dockingブランチを参照...

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
	dxCommon_->GetCommandList()->SetDescriptorHeaps(descriptorIndex_ + 1, descriptorHeaps);
}

void ImGuiManager::End() {
	ImGui::Render();

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList());
}

ImGuiManager* ImGuiManager::GetInstaince() {
	static ImGuiManager instance;
	return &instance;
}
