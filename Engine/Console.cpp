#include "Console.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "externals/imgui/imgui.h"
#include "MyEngine.h"
#include "Environment.h"
#include "ExecutionSpeed.h"

#include <Attribute.h>

//=========================================================================================
// static variables
//=========================================================================================
const Vector4f Console::commentOutColor = { 0.0f, 0.55f, 0.0f, 1.0f };
const Vector4f Console::errorColor      = { 0.8f, 0.0f, 0.0f, 1.0f };
const Vector4f Console::warningColor    = { 0.8f, 0.8f, 0.0f, 1.0f };

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
}

void Console::Term() {
	logDatas_.clear();
}

void Console::Update() {
	ImGui::ShowDemoWindow();

	// game window
	OutputScene();

	OutputLog();
	OutputOutliner();
	OutputPerformance();
}

void Console::SetLog(const std::string& log, const Vector4f& color) {
	// logの追加
	logDatas_.push_front({log, color});

	if (logDatas_.size() >= kMaxLogData_) {
		// 一番古いログの削除
		logDatas_.pop_back();
	}
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
		ImGui::BeginChild(ImGui::GetID((void*)0), wndSize, ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);

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

	static std::string name;

	// TODO: 名前の重複参照の対策
	
	for (const auto& object : Outliner_) {
		bool isSelect = (object == selectedAttribute_);

		if (ImGui::Selectable(object->GetName().c_str(), isSelect)) {
			selectedAttribute_ = object;
			name = selectedAttribute_->GetName();
		}
	}

	ImGui::End();

	// attribute
	ImGui::Begin("Attribute", &isOpenWindow, ImGuiWindowFlags_NoCollapse);
	if (selectedAttribute_ != nullptr) {

		ImGui::Text(selectedAttribute_->GetName().c_str());
		
		//ImGui::InputText("name", name.data(), 20);
		//// HACK: 文字数0にすると止まる
		//// std::stringのsize以上に入力するとsize以上が表示されない

		//if (ImGui::Button("set")) {
		//	if (name.empty()) { //!< nameが空だった場合
		//		name = selectedAttribute_->GetName(); //!< 元に戻す
		//		SetLog("name can't set empty", errorColor);

		//	} else {
		//		selectedAttribute_->SetName(name.data());
		//	}
		//}
		
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();

		selectedAttribute_->SetAttributeImGui();
	}
	ImGui::End();
}


