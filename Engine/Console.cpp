#include "Console.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "imgui.h"
#include "MyEngine.h"
#include "Environment.h"
#include "Performance.h"
#include "DirectXCommon.h"
#include "TextureManager.h"

#include <Attribute.h>
#include <format>

//-----------------------------------------------------------------------------------------
// using namespace
//-----------------------------------------------------------------------------------------
using namespace DxObject;

//=========================================================================================
// static variables
//=========================================================================================
const Color4f Console::commentOutColor = { 0.0f, 0.55f, 0.0f, 1.0f };
const Color4f Console::errorColor      = { 0.8f, 0.0f, 0.0f, 1.0f };
const Color4f Console::warningColor    = { 0.8f, 0.8f, 0.0f, 1.0f };
const Color4f Console::defaultColor    = { 0.98f, 0.98f, 0.98f, 1.0f };

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
	dxCommon_ = MyEngine::GetDxCommon();

	sceneTexture_ = MyEngine::CreateRenderTexture("sceneTexture", kWindowWidth, kWindowHeight);

	debugCamera_ = std::make_unique<Camera3D>();
	debugCamera_->SetAttributeName("sceneCamera");
	SetAttribute(debugCamera_.get());

}

void Console::Term() {
	logDatas_.clear();
	debugCamera_.reset();
}

void Console::Update() {

	OutputMenu();

	if (isOutputConsole_) {

		windowFlags_ = ImGuiWindowFlags_NoCollapse;

		if (isFix_) {
			windowFlags_ |= ImGuiWindowFlags_NoMove;
			windowFlags_ |= ImGuiWindowFlags_NoResize;
		}

		// game window
		OutputScene();
		OutputGame();

		OutputLog();
		OutputOutliner();
		OutputPerformance();
		OutputSystem();
		OutputAssets();
	}
}

void Console::Log(const std::string& log, const Color4f& color) {
	// logの追加
	logDatas_.push_front({log, color});

	while (logDatas_.size() >= kMaxLogData_) {
		// 一番古いログの削除
		logDatas_.pop_back();
	}
}

void Console::CheckEraseAttribute(Attribute* obj) {
	if (obj == selectedAttribute_) { //!< 削除するattributeが今選択されてるattributeだった場合
		selectedAttribute_ = nullptr;
	}
}

void Console::OutputTexture(const std::string& name, const D3D12_GPU_DESCRIPTOR_HANDLE& srvHandleGPU) {
	if (isOutputConsole_) {
		static bool isOpenWindow = true;
		ImGui::Begin(name.c_str(), &isOpenWindow, windowFlags_);

		SetTextureImGui(srvHandleGPU);

		ImGui::End();
	}
}

void Console::OutputDefferedTextures(const std::string& name, uint32_t indexSize, const D3D12_GPU_DESCRIPTOR_HANDLE textureHandles[8]) {

	if (!isOutputConsole_) { return; }
	if (indexSize == 0) { return; }

	static bool isOpenWindow = true;
	ImGui::Begin(name.c_str(), &isOpenWindow, windowFlags_);

	// タブ等を排除した全体のwindowSize計算
	ImVec2 regionMax  = ImGui::GetWindowContentRegionMax();
	ImVec2 regionMin  = ImGui::GetWindowContentRegionMin();
	ImVec2 windowSize = { regionMax.x - regionMin.x, regionMax.y - regionMin.y };

	// windowの分割数の設定
	const uint32_t kSubdivisionWidth  = 2;
	const uint32_t kSubdivisionHeight = (indexSize / kSubdivisionWidth) + (indexSize % kSubdivisionWidth);

	// windowを分割したsizeの計算
	ImVec2 windowDivisionSize = { windowSize.x / kSubdivisionWidth, windowSize.y / kSubdivisionHeight };

	for (uint32_t h = 0; h < kSubdivisionHeight; ++h) {
		for (uint32_t w = 0; w < kSubdivisionWidth; ++w) {

			uint32_t index = w + h * kSubdivisionWidth;

			//!< 配列を超える場合, 終了
			if (index >= indexSize) { break; }

			// 画像アス比と分割したWindowアス比の計算
			float textureAspectRatio = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight);
			float windowAspectRatio  = windowDivisionSize.x / windowDivisionSize.y;

			// 出力する画像サイズの設定
			ImVec2 displayTextureSize = windowDivisionSize;

			// 画像サイズの調整
			if (textureAspectRatio <= windowAspectRatio) {
				displayTextureSize.x *= textureAspectRatio / windowAspectRatio;

			} else {
				displayTextureSize.y *= windowAspectRatio / textureAspectRatio;
			}

			// 出力場所の調整
			ImVec2 topLeft = {
				(windowDivisionSize.x - displayTextureSize.x) * 0.5f + regionMin.x + displayTextureSize.x * w,
				(windowDivisionSize.y - displayTextureSize.y) * 0.5f + regionMin.y + displayTextureSize.y * h,
			};


			ImGui::SetCursorPos(topLeft);

			ImGui::Image((ImTextureID)textureHandles[index].ptr, displayTextureSize);

		}
	}

	ImGui::End();
	
}

//=========================================================================================
// private methods
//=========================================================================================

void Console::OutputMenu() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Settings")) {

			ImGui::Checkbox("DisplayConsole", &isOutputConsole_);

			// windowFlags_
			ImGui::Checkbox("Fix(lock size and move)", &isFix_);


			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void Console::OutputScene() {
	static bool isOpenWindow = true;
	ImGui::Begin("Scene", &isOpenWindow, windowFlags_);
	// todo: window開かれてる状態を保存させる. foucusとは違う
	isFocusDebugScene_; //!< 名前もfoucusからいい感じに変更させる

	SetTextureImGui(sceneTexture_->GetGPUHandleSRV());

	ImGui::End();
}

void Console::OutputGame() {
	static bool isOpenWindow = true;
	ImGui::Begin("Game", &isOpenWindow, windowFlags_);
	// this->windowがフォーカスされてるかどうか
	isFocusGameScene_ = ImGui::IsWindowFocused();

	SetTextureImGui(MyEngine::GetTextureHandleGPU("offscreen"));

	ImGui::End();
}

void Console::OutputLog() {
	//! 開いておく
	static bool isOpenWindow = true;
	ImGui::Begin("Log", &isOpenWindow, windowFlags_);

	if (ImGui::BeginTabBar("LogTab")) {

		if (ImGui::BeginTabItem("Console")) {

			ImVec2 cntRegionMax = ImGui::GetWindowContentRegionMax();
			ImVec2 cntRegionMin = ImGui::GetWindowContentRegionMin();
			ImVec2 wndSize = { cntRegionMax.x - cntRegionMin.x, cntRegionMax.y - cntRegionMin.y };

			
			ImGui::BeginChild(ImGui::GetID((void*)0), wndSize, ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
			for (auto& log : logDatas_) {
				ImGui::TextColored({ log.color.r, log.color.g, log.color.b, log.color.a }, log.log.c_str());
			}
			ImGui::EndChild();
			

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Printf")) {

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void Console::OutputPerformance() {

	// systemの状態によってstyleを変える
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 defaultWindowColor = style.Colors[ImGuiCol_WindowBg]; //!< 後で元に戻すので保存

	if (isUpdateRequired_) {
		// windowの色を変更
		style.Colors[ImGuiCol_WindowBg] = ImGuiManager::ToImVec4({ 45, 5, 8, 255 });
	}

	static bool isOpenWindow = true;
	ImGui::Begin("Performance", &isOpenWindow, windowFlags_ | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

	float framesPerSec = Performance::GetDeltaTime(SecondsUnit::s);

	ImGui::Text("exec speed / frame: %.6f", framesPerSec);
	ImGui::SameLine();
	ImGui::Text("FPS: %.0f", 1.0f / framesPerSec);

	ImGui::End();

	style.Colors[ImGuiCol_WindowBg] = defaultWindowColor;
}

void Console::OutputOutliner() {
	// Inspectorの選択window
	static bool isOpenWindow = true;
	ImGui::Begin("Outliner", &isOpenWindow, windowFlags_);
	
	for (const auto& object : attributes_) {
		OutlinerAttribute(object);
	}

	ImGui::End();

	// attribute
	ImGui::Begin("Attribute", &isOpenWindow, windowFlags_);
	if (selectedAttribute_ != nullptr) {

		ImGui::SeparatorText(selectedAttribute_->GetName().c_str());

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

		selectedAttribute_->SetAttributeImGui();

	}

	ImGui::End();
}

void Console::OutputSystem() {
	static bool isOpenWindow = true;
	ImGui::Begin("System", &isOpenWindow, windowFlags_);

	//!< 更新処理関係
	if (ImGui::CollapsingHeader("Process")) {
		if (isUpdateRequired_) { //!< 更新処理が有効な時
			if (ImGui::Button("stop")) {
				isUpdateRequired_ = false;
			}

		} else {
			if (ImGui::Button("start")) {
				isUpdateRequired_ = true;
			}

		}

		ImGui::Spacing();
	}

	//!< Inputが機能してるかの確認
	if (ImGui::CollapsingHeader("Input")) {

		/*if (ImGui::TreeNode("Keyboard")) {

			auto keyboard = Input::GetInstance()->GetKeyboardInput();

			ImGui::TreePop();
		}*/

		if (ImGui::TreeNode("Mouse")) {

			auto mouse = Input::GetInstance()->GetMouseInput();

			ImGui::Text("[position] x: %d, y: %d", mouse->GetMousePos().x, mouse->GetMousePos().y);
			ImGui::Text("[delta]    x: %d, y: %d", mouse->GetDeltaMousePos().x, mouse->GetDeltaMousePos().y);

			ImGui::TreePop();
		}
	}

	//!< descriptorの使用済みの数, 最大数の確認
	if (ImGui::CollapsingHeader("DescriptorHeaps")) {
		auto descriptorHeaps = dxCommon_->GetDescriptorsObj();

		if (ImGui::TreeNode("RTV")) {
			ImGui::Text(
				"used: %d / max: %d",
				descriptorHeaps->GetUsedDescriptor(RTV),
				descriptorHeaps->GetIndexSize(RTV)
			);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("SRV & UAV")) {

			ImGui::Text(
				"used: %d / max: %d",
				descriptorHeaps->GetUsedDescriptor(SRV),
				descriptorHeaps->GetIndexSize(SRV)
			);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("DSV")) {

			ImGui::Text(
				"used: %d / max: %d",
				descriptorHeaps->GetUsedDescriptor(DSV),
				descriptorHeaps->GetIndexSize(DSV)
			);

			ImGui::TreePop();
		}

	}

	ImGui::End();
}

void Console::OutputAssets() {
	ImGui::Begin("Asserts", nullptr, windowFlags_);



	ImGui::End();

}

void Console::SetTextureImGui(const D3D12_GPU_DESCRIPTOR_HANDLE& texture) {
	
	// タブ等を排除した全体のwindowSize計算
	ImVec2 regionMax  = ImGui::GetWindowContentRegionMax();
	ImVec2 regionMin  = ImGui::GetWindowContentRegionMin();
	ImVec2 windowSize = { regionMax.x - regionMin.x, regionMax.y - regionMin.y };
	
	// 画像アス比と分割したWindowアス比の計算
	float textureAspectRatio = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight);
	float windowAspectRatio  = windowSize.x / windowSize.y;
	
	// 出力する画像サイズの設定
	ImVec2 displayTextureSize = windowSize;
	
	// 画像サイズの調整
	if (textureAspectRatio <= windowAspectRatio) {
		displayTextureSize.x *= textureAspectRatio / windowAspectRatio;
	
	} else {
		displayTextureSize.y *= windowAspectRatio / textureAspectRatio;
	}
	
	// 出力場所の調整
	ImVec2 leftTop = {
		(windowSize.x - displayTextureSize.x) * 0.5f + regionMin.x,
		(windowSize.y - displayTextureSize.y) * 0.5f + regionMin.y,
	};
	
	ImGui::SetCursorPos(leftTop);
	ImGui::Image((ImTextureID)texture.ptr, displayTextureSize);
	
}

void Console::OutlinerAttribute(Attribute* attribute) {

	// attributeが選択されてるか
	bool isSelect = (attribute == selectedAttribute_);

	// selectableに表示される名前
	std::string label = attribute->GetName();

	// 重複対策, ptrの値をstackIdとする
	label += "##" + std::format("{:p}", reinterpret_cast<void*>(attribute));

	if (attribute->GetNode().empty()) { //!< node構築がされてない場合
		if (ImGui::Selectable(label.c_str(), isSelect)) { //!< selectされた場合
			selectedAttribute_ = attribute;
		}

	} else {

		ImGuiTreeNodeFlags flags
			= ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_OpenOnArrow;

		if (isSelect) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		if (ImGui::TreeNodeEx(label.c_str(), flags)) {
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) { //!< selectされた場合
				selectedAttribute_ = attribute;
			}

			for (const auto& node : attribute->GetNode()) {
				OutlinerAttribute(node);
			}

			ImGui::TreePop();
		}
	}
}

