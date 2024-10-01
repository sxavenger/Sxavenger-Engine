#include "Console.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// attribute
#include <Engine/System/Attribute.h>

// engine
#include <Engine/System/Sxavenger.h>
#include <Engine/System/Performance.h>
#include <Lib/Environment.h>

//=========================================================================================
// static variables
//=========================================================================================
// template color
const Color4f Console::commentOutColor = { 0.0f, 0.55f, 0.0f, 1.0f };
const Color4f Console::errorColor      = { 0.8f, 0.0f, 0.0f, 1.0f };
const Color4f Console::warningColor    = { 0.8f, 0.8f, 0.0f, 1.0f };
const Color4f Console::defaultColor    = { 0.98f, 0.98f, 0.98f, 1.0f };

// console name
const std::string Console::kConsoleName_ = "Sxavenger EngineConsole";

////////////////////////////////////////////////////////////////////////////////////////////
// Console class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Console::Init() {

	sceneTexture_ = std::make_unique<RenderTexture>();
	sceneTexture_->Create(Sxavenger::GetDxCommon(), kWindowSize);

	debugCamera_ = std::make_unique<DebugCamera3D>();
	debugCamera_->SetProjection(0.05f, static_cast<float>(kWindowSize.x) / static_cast<float>(kWindowSize.y), 0.1f, 10000.0f);
	//debugCamera_->SetAttributeName("sceneCamera");
	//SetAttribute(debugCamera_.get());

	// windowFlagの初期化
	windowFlags_ = 0;
	windowFlags_ |= ImGuiWindowFlags_NoCollapse;
	windowFlags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	windowFlags_ |= ImGuiWindowFlags_NoMove;
	windowFlags_ |= ImGuiWindowFlags_NoResize;

#ifndef _DEBUG
	isDisplayConsole_ = false;
#endif // _DEBUG
}

void Console::Term() {
	logs_.clear();
	debugCamera_.reset();
	sceneTexture_.reset();

	//!< imguiの外部書き込みしない場合はnullに設定
	if (!isOutputImGuiFile_) {
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = NULL;
	}
}

void Console::Update() {
#ifndef _DEBUG
	return;
#endif // _DEBUG

	/*ImGui::Begin("test window");
	ImGui::ShowStyleEditor();
	ImGui::End();*/

	DisplayMenu();

	if (isDisplayConsole_) {

		dockingID_ = ImGui::GetID(kConsoleName_.c_str());

		//* display scenes *//
		DisplayGame();
		DisplayScene();

		//* display outliner & attribute *//
		DisplayOutliner();
		DisplayAttribute();

		//* display log *//
		DisplayLogs();

		//* display systems *//
		DisplaySystem();
		DisplayPerformance();
		
		//* debug camera *//
		debugCamera_->Update();
	}

}

void Console::DockingConsole() const {
	ImGui::SetNextWindowDockID(dockingID_, ImGuiCond_FirstUseEver);
}

void Console::SetAttribute(Attribute* attribute) {
	Assert(!attribute->GetName().empty());
	attributes_.emplace_back(attribute);
}

void Console::CheckEraseAttribute(Attribute* obj) {
	if (obj == selectedAttribute_) { //!< 削除するattributeが今選択されてるattributeだった場合
		selectedAttribute_ = nullptr;
		// todo: 選択されてる上のやつが再度選択されるように
	}

	attributes_.remove(obj);
}

void Console::Log(const std::string& log, const Color4f& color) {
	// logの追加
	logs_.emplace_front(LogData{log, color});

	while (logs_.size() >= kMaxLog_) {
		logs_.pop_back(); //!< 一番古いログの削除
	}
}

void Console::BeginPrintf() {
	if (!isDisplayConsole_) {
		return;
	}
	DockingConsole();
	static bool isOpenWindow = true;
	ImGui::Begin("Printf##SxavengerConsole", &isOpenWindow, windowFlags_);
}

void Console::EndPrintf() {
	if (!isDisplayConsole_) {
		return;
	}
	ImGui::End();
}

void Console::BreakPoint(const std::source_location& location) {
	if (!isDisplayConsole_) {
		return;
	}

	// 更新処理の停止
	isUpdateRequired_ = false;
	updateLimit_      = 0;

	// logの出力
	std::string message = {};
	message += "[console] break point called.\n";
	message += std::format(" filename: {} \n", location.file_name());
	message += std::format(" function: {} \n", location.function_name());
	message += std::format(" line:     {} \n", location.line());

	Log(message, commentOutColor);

}

Console* Console::GetInstance() {
	static Console instance;
	return &instance;
}

void Console::DisplayTextureForConsole(const std::string& title, const D3D12_GPU_DESCRIPTOR_HANDLE handle) {
	if (!isDisplayConsole_) {
		return;
	}
	DockingConsole();
	static bool isOpenWindow = true;

	ImGui::Begin(title.c_str(), &isOpenWindow, windowFlags_);
	DisplayTextureImGuiWindow(handle);
	ImGui::End();
}

//=========================================================================================
// display consoles methods
//=========================================================================================

void Console::DisplayMenu() {
	ImGui::BeginMainMenuBar();
	
	if (ImGui::BeginMenu("Configs")) {

		//* console config *//
		ImGui::SeparatorText("console config");
		ImGui::Checkbox("Display Console", &isDisplayConsole_);
		ImGui::Checkbox("Output imgui layout", &isOutputImGuiFile_);

		//* window config *//
		ImGui::SeparatorText("window config");
		/*windowFlags_ |= ImGuiWindowFlags_NoCollapse;*/
		//!< default settings
		
		//* lock window
		ImGuiWindowFlags lockWindow = 0;
		lockWindow |= ImGuiWindowFlags_NoMove;
		lockWindow |= ImGuiWindowFlags_NoResize;
		ImGui::CheckboxFlags("lock window position & size", &windowFlags_, lockWindow);

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Layout")) {

		//!< ImGui公式のstyleに関するeditor
		ImGui::ShowStyleEditor();

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}

void Console::DisplayScene() {
	DockingConsole();
	static bool isOpenWindow = true;
	ImGui::Begin("Scene", &isOpenWindow, windowFlags_);
	// todo: window開かれてる状態を保存させる

	DisplayTextureImGuiWindow(sceneTexture_->GetGPUHandleSRV());

	ImGui::End();
}

void Console::DisplayGame() {
	DockingConsole();
	static bool isOpenWindow = true;
	ImGui::Begin("Game", &isOpenWindow, windowFlags_);
	// this->windowがフォーカスされてるかどうか

	DisplayTextureImGuiWindow(Sxavenger::GetTextureHandleGPU("main"));

	ImGui::End();
}

void Console::DisplayOutliner() {
	DockingConsole();
	static bool isOpenWindow = true;
	ImGui::Begin("Outliner", &isOpenWindow, windowFlags_);

	for (const auto& object : attributes_) {
		SelectableAttribute(object);
	}

	ImGui::End();
}

void Console::DisplayAttribute() {
	DockingConsole();
	static bool isOpenWindow = true;
	ImGui::Begin("Attribute", &isOpenWindow, windowFlags_);

	if (selectedAttribute_ != nullptr) {
		ImGui::SeparatorText(selectedAttribute_->GetName().c_str());
		// todo: attributeから名前の変更

		selectedAttribute_->SetAttributeImGui();
	}

	ImGui::End();
}

void Console::DisplayLogs() {
	DockingConsole();
	static bool isOpenWindow = true;
	ImGui::Begin("Console", &isOpenWindow, windowFlags_);

	
	ImVec2 cntRegionMax = ImGui::GetWindowContentRegionMax();
	ImVec2 cntRegionMin = ImGui::GetWindowContentRegionMin();
	ImVec2 wndSize = { cntRegionMax.x - cntRegionMin.x, cntRegionMax.y - cntRegionMin.y };

	ImGui::BeginChild(ImGui::GetID((void*)0), wndSize, ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
	for (auto& log : logs_) {
		ImGui::TextColored({ log.color.r, log.color.g, log.color.b, log.color.a }, log.log.c_str());
	}
	ImGui::EndChild();

	if (ImGui::Button("clear")) {
		logs_.clear();
	}

	ImGui::SameLine();

	if (ImGui::Button("line")) {
		Log("--------------------------------------------------------", commentOutColor);
	}

	ImGui::End();

	ImGui::Begin("Printf##SxavengerConsole", &isOpenWindow, windowFlags_);
	ImGui::End();
}

void Console::DisplaySystem() {
	DockingConsole();
	static bool isOpenWindow = true;
	ImGui::Begin("System", &isOpenWindow, windowFlags_);

	//!< 更新処理関係
	if (ImGui::CollapsingHeader("Process")) {
		if (updateLimit_ == std::nullopt) { //!< 更新処理の制限がない場合
			if (ImGui::Button("stop")) {
				updateLimit_ = 0; //!< 制限の代入
			}

		} else {
			if (ImGui::Button("start")) {
				updateLimit_ = std::nullopt; //!< 制限なし
			}

			ImGui::Spacing();

			if (ImGui::Button("next frame")) {
				updateLimit_ = 1; //!< 次にframeまで実行
			}
			
			// 何フレーム更新するかの決定処理
			static uint32_t count = 0;

			ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true); //!< 長押しで更新できるようにflagを設定

			if (ImGui::ArrowButton("##left console", ImGuiDir_Left)) {
				if (count != 0) { count--; }
			}

			ImGui::SameLine();

			if (ImGui::ArrowButton("##right console", ImGuiDir_Right)) {
				count++;
			}

			ImGui::PopItemFlag();
			ImGui::SameLine();

			ImGui::Text("update count : %u <frame>", count);
			// todo: inputTextの追加

			ImGui::SameLine();

			if (ImGui::Button("execute")) {
				updateLimit_ = count;
				count = 0;
			}

			if (updateLimit_) {
				ImGui::Text("update count remaining : %u <frame>", updateLimit_.value());
			}
		}

		// 最終的に更新処理をするかの確定
		isUpdateRequired_ = true;

		if (updateLimit_) { //!< updateLimitがある場合
			if (updateLimit_.value() == 0) { //!< 更新処理の回数が0の時
				isUpdateRequired_ = false; //!< 更新処理のstop

			} else {
				updateLimit_.value() -= 1;
			}
		}

		ImGui::Spacing();
	}

	//!< Inputが機能してるかの確認
	if (ImGui::CollapsingHeader("Input")) {

		auto input = Sxavenger::GetInput();

		/*if (ImGui::TreeNode("Keyboard")) {

			auto keyboard = Input::GetInstance()->GetKeyboardInput();

			ImGui::TreePop();
		}*/

		if (ImGui::TreeNode("Mouse")) {

			auto mouse = input->GetMouseInput();

			ImGui::Text("[position] x: %d, y: %d", mouse->GetMousePos().x, mouse->GetMousePos().y);
			ImGui::Text("[delta]    x: %d, y: %d", mouse->GetDeltaMousePos().x, mouse->GetDeltaMousePos().y);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Gamepad")) {

			for (uint32_t i = 0; i < input->GetGamepadNum(); ++i) {

				auto gamepad = input->GetGamepadInput(i);
				std::string label = std::format("[{}]", i);

				if (ImGui::TreeNode(label.c_str())) {

					if (gamepad->IsConnect()) {
						ImGui::Text("[LStick]:          x: %d, y: %d", gamepad->GetLStick().x, gamepad->GetLStick().y);

					} else {
						ImGui::Text("not connect.");
					}

					ImGui::TreePop();
				}
			}
			
			ImGui::TreePop();
		}
	}

	//!< descriptorの使用済みの数, 最大数の確認
	if (ImGui::CollapsingHeader("DescriptorHeaps")) {
		auto dxCommon = Sxavenger::GetDxCommon();
		auto descriptorHeaps = dxCommon->GetDescriptorsObj();

		if (ImGui::TreeNode("RTV")) {
			ImGui::Text(
				"used: %d / max: %d",
				descriptorHeaps->GetDescriptorPool(RTV)->GetUsedDescriptorsCount(),
				descriptorHeaps->GetDescriptorPool(RTV)->GetDescriptorMaxCount()
			);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("SRV & UAV")) {

			ImGui::Text(
				"used: %d / max: %d",
				descriptorHeaps->GetDescriptorPool(CBV_SRV_UAV)->GetUsedDescriptorsCount(),
				descriptorHeaps->GetDescriptorPool(CBV_SRV_UAV)->GetDescriptorMaxCount()
			);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("DSV")) {

			ImGui::Text(
				"used: %d / max: %d",
				descriptorHeaps->GetDescriptorPool(DSV)->GetUsedDescriptorsCount(),
				descriptorHeaps->GetDescriptorPool(DSV)->GetDescriptorMaxCount()
			);

			ImGui::TreePop();
		}

	}

	ImGui::End();
}

void Console::DisplayPerformance() {

	// systemの状態によってstyleを変える
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 defaultWindowColor = style.Colors[ImGuiCol_WindowBg]; //!< 後で元に戻すので保存

	if (isUpdateRequired_) {
		// windowの色を変更
		style.Colors[ImGuiCol_WindowBg] = ImGuiManager::ToImVec4({ 45, 5, 8, 255 });
	}

	DockingConsole();
	static bool isOpenWindow = true;
	ImGui::Begin("Performance", &isOpenWindow, windowFlags_ | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

	DeltaTimePoint framesPerSec = Performance::GetDeltaTime(SecondsUnit::s);

	ImGui::Text("exec speed / frame: %.6f", framesPerSec.time);
	ImGui::SameLine();
	ImGui::Text("FPS: %.0f", 1.0f / framesPerSec.time);

	ImGui::End();

	style.Colors[ImGuiCol_WindowBg] = defaultWindowColor;
}

void Console::DisplayTextureImGuiWindow(const D3D12_GPU_DESCRIPTOR_HANDLE texture) {

	// タブ等を排除した全体のwindowSize計算
	ImVec2 regionMax  = ImGui::GetWindowContentRegionMax();
	ImVec2 regionMin  = ImGui::GetWindowContentRegionMin();
	ImVec2 windowSize = { regionMax.x - regionMin.x, regionMax.y - regionMin.y };
	
	// 画像アス比と分割したWindowアス比の計算
	float textureAspectRatio = static_cast<float>(kWindowSize.x) / static_cast<float>(kWindowSize.y);
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

void Console::SelectableAttribute(Attribute* attribute) {

	// attributeが選択されてるか
	bool isSelect = (attribute == selectedAttribute_);

	// selectableに表示される名前の設定
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
				SelectableAttribute(node);
			}

			ImGui::TreePop();
		}
	}
}
