#include "Console.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "externals/imgui/imgui.h"
#include "MyEngine.h"
#include "Environment.h"
#include "ExecutionSpeed.h"

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
}

void Console::Update() {
	OutputConsole();
	OutputLog();
	OutputSpeed();
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

void Console::OutputConsole() {
	//! 開いておく
	static bool isOpenWindow = true;
	ImGui::Begin("Console", &isOpenWindow, ImGuiWindowFlags_NoCollapse);

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

void Console::OutputSpeed() {
	static bool isOpenWindow = true;
	ImGui::Begin("Speed", &isOpenWindow);

	ImGui::Text("speed(s): %.6f", ExecutionSpeed::freamsParSec_);
	ImGui::Text("FPS: %.1f", 1.0f / ExecutionSpeed::freamsParSec_);

	ImGui::End();
}


