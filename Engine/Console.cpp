#include "Console.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "externals/imgui/imgui.h"
#include "MyEngine.h"
#include "Environment.h"
#include "ExecutionSpeed.h"

#include <Outliner.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Console class methods
////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************************
// 一時的にシングルトンとしている。
//*****************************************************************************************

Console* Console::GetInstance() {
	static Console instance;
	return &instance;
}

void Console::Init() {
	logDatas_.resize(kMaxLogData_);
}

void Console::Term() {
	logDatas_.clear();
}

void Console::Update() {
	ImGui::ShowDemoWindow();

	// game window
	OutputScene();

	OutputLog();
	OutputPerformance();
	OutputOutliner();
}

void Console::SetLog(const std::string& log, const Vector4f& color) {
	// logの追加
	logDatas_.push_front({log, color});

	// 一番古いログの削除
	logDatas_.pop_back();
}

//=========================================================================================
// private methods
//=========================================================================================

void Console::OutputScene() {
	ImGui::Begin("Scene", &isOpenDebugScene_, ImGuiWindowFlags_NoCollapse);

	//タブ等を除いたウィンドウのサイズを取得(計算)
	ImVec2 cntRegionMax = ImGui::GetWindowContentRegionMax();
	ImVec2 cntRegionMin = ImGui::GetWindowContentRegionMin();
	ImVec2 wndSize = { cntRegionMax.x - cntRegionMin.x, cntRegionMax.y - cntRegionMin.y };

	//元のアス比とImGuiウィンドウのアス比を比較
	float outerWindowAspectRatio = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight);
	float innerWindowAspectRatio = wndSize.x / wndSize.y;
	ImVec2 finalWndSize = wndSize;

	//横幅が大きかったら縦基準で画像サイズを決定
	if (outerWindowAspectRatio <= innerWindowAspectRatio) {
		finalWndSize.x *= outerWindowAspectRatio / innerWindowAspectRatio;
	}
	//縦幅が大きかったら横基準で画像サイズを決定
	else {
		finalWndSize.y *= innerWindowAspectRatio / outerWindowAspectRatio;
	}

	//画像を中央に持ってくる
	ImVec2 topLeft = { (wndSize.x - finalWndSize.x) * 0.5f + cntRegionMin.x,
						(wndSize.y - finalWndSize.y) * 0.5f + cntRegionMin.y };
	ImGui::SetCursorPos(topLeft);

	D3D12_GPU_DESCRIPTOR_HANDLE handle = MyEngine::GetTextureHandleGPU("offscreen");
	ImGui::Image((ImTextureID)handle.ptr, finalWndSize);

	ImGui::End();
}

void Console::OutputLog() {
	//! 開いておく
	static bool isOpenWindow = true;
	ImGui::Begin("Log", &isOpenWindow);

	ImVec2 cntRegionMax = ImGui::GetWindowContentRegionMax();
	ImVec2 cntRegionMin = ImGui::GetWindowContentRegionMin();
	ImVec2 wndSize = { cntRegionMax.x - cntRegionMin.x, cntRegionMax.y - cntRegionMin.y };

	{
		ImGui::BeginChild(ImGui::GetID((void*)0), wndSize, ImGuiWindowFlags_NoTitleBar);

		for (auto& log : logDatas_) {
			ImGui::TextColored({log.color.r, log.color.g, log.color.b, log.color.a}, log.log.c_str());
		}

		ImGui::EndChild();
	}

	ImGui::End();
}

void Console::OutputPerformance() {
	static bool isOpenWindow = true;
	ImGui::Begin("Performance", &isOpenWindow, ImGuiWindowFlags_NoTitleBar);

	ImGui::Text("exec speed / frame: %.6f", ExecutionSpeed::freamsParSec_);
	ImGui::SameLine();
	ImGui::Text("FPS: %.1f", 1.0f / ExecutionSpeed::freamsParSec_);

	ImGui::End();
}

void Console::OutputOutliner() {
	// Inspectorの選択window
	static bool isOpenWindow = true;
	ImGui::Begin("Outliner", &isOpenWindow, ImGuiWindowFlags_NoCollapse);

	// TODO: 名前の重複参照の対策
	
	for (const auto& object : Outliners_) {
		bool isSelect = (object == selectedOutliner_);

		std::string name = object->GetName();

		if (ImGui::Selectable(name.c_str(), isSelect)) {
			selectedOutliner_ = object;
		}
	}

	ImGui::End();

	// inspector
	ImGui::Begin("Inspector", &isOpenWindow, ImGuiWindowFlags_NoCollapse);
	if (selectedOutliner_ != nullptr) {
		ImGui::TextColored({1.0f, 1.0f, 1.0f, 1.0f}, selectedOutliner_->GetName().c_str());
		selectedOutliner_->SetOutlinerImGui();
	}
	ImGui::End();
}


