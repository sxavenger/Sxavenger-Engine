#include "BetaConsole.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* MonoBehavior
#include "MonoBehavior.h"

//* engine
#include <Engine/System/Performance.h>

//* Lib
#include <Lib/Environment.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BetaConsolePipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BetaConsolePipeline::Init() {
	
	{
		blobs_[kConsoleLocal] = std::make_unique<GraphicsBlob>();
		blobs_[kConsoleLocal]->Create(L"monobehavior/consoleLocal.vs.hlsl", GRAPHICS_VERTEX);
		blobs_[kConsoleLocal]->Create(L"monobehavior/consoleLocal.ps.hlsl", GRAPHICS_PIXEL);

		pipelines_[kConsoleLocal] = std::make_unique<GraphicsPipeline>();

		GraphicsRootSignatureDesc descRoot;
		descRoot.SetCBV(0, VISIBILITY_ALL, 10);   //!< camera
		descRoot.SetSRV(1, VISIBILITY_PIXEL, 0);  //!< Diffuse
		descRoot.SetSampler(0, MODE_WRAP, VISIBILITY_PIXEL, 0);

		pipelines_[kConsoleLocal]->CreateRootSignature(Sxavenger::GetDevicesObj(), descRoot);

		GraphicsPipelineDesc desc;
		desc.CreateDefaultDesc();

		pipelines_[kConsoleLocal]->CreatePipeline(Sxavenger::GetDevicesObj(), blobs_[kConsoleLocal].get(), desc);
	}

	{
		blobs_[kDefault_Diffuse] = std::make_unique<GraphicsBlob>();
		blobs_[kDefault_Diffuse]->Create(L"monobehavior/default.vs.hlsl", GRAPHICS_VERTEX);
		blobs_[kDefault_Diffuse]->Create(L"monobehavior/diffuse.ps.hlsl", GRAPHICS_PIXEL);

		pipelines_[kDefault_Diffuse] = std::make_unique<GraphicsPipeline>();

		GraphicsRootSignatureDesc descRoot;
		descRoot.SetCBV(0, VISIBILITY_ALL, 10);   //!< camera
		descRoot.SetCBV(1, VISIBILITY_VERTEX, 0); //!< Transform
		descRoot.SetSRV(2, VISIBILITY_PIXEL, 0);  //!< Diffuse
		descRoot.SetSampler(0, MODE_WRAP, VISIBILITY_PIXEL, 0);

		pipelines_[kDefault_Diffuse]->CreateRootSignature(Sxavenger::GetDevicesObj(), descRoot);

		GraphicsPipelineDesc desc;
		desc.CreateDefaultDesc();

		pipelines_[kDefault_Diffuse]->CreatePipeline(Sxavenger::GetDevicesObj(), blobs_[kDefault_Diffuse].get(), desc);
	}

}

void BetaConsolePipeline::Term() {
	for (uint32_t i = 0; i < kCountOfPipelineType; ++i) {
		blobs_[i].reset();
		pipelines_[i].reset();
	}
}

void BetaConsolePipeline::SetPipeline(PipelineType type) {
	auto commandList = Sxavenger::GetCommandList();
	pipelines_[type]->SetPipeline(commandList);
}

//=========================================================================================
// BetaConsole class // static variables
//=========================================================================================

const Color4f BetaConsole::kCommentOutColor = { 0.0f, 0.55f, 0.0f, 1.0f };
const Color4f BetaConsole::kErrorColor      = { 0.8f, 0.0f, 0.0f, 1.0f };
const Color4f BetaConsole::kWarningColor    = { 0.8f, 0.8f, 0.0f, 1.0f };
const Color4f BetaConsole::kDefaultColor    = { 0.98f, 0.98f, 0.98f, 1.0f };

const std::string BetaConsole::kConsoleName_      = "Sxavenger Engine Console";
const std::string BetaConsole::kImGuiIniFilepath_ = "imgui.ini";

////////////////////////////////////////////////////////////////////////////////////////////
// BetaConsole class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BetaConsole::Init() {
	InitConfig();
	InitRenderTarget();

	pipeline_.Init();

	//!< imguiの書き込み, 読み込みを手動に変更
	ImGuiIO& io    = ImGui::GetIO();
	io.IniFilename = NULL;
	ImGui::LoadIniSettingsFromDisk(kImGuiIniFilepath_.c_str());
}

void BetaConsole::Term() {
	TermRenderTarget();

	pipeline_.Term();

	//!< .iniに書き込み
	if (isOutputImGuiIni_) {
		ImGui::SaveIniSettingsToDisk(kImGuiIniFilepath_.c_str());
	}
}

void BetaConsole::Update() {

	DisplayMainMenu();

	if (isDisplayConsole_) {

		//!< docking用IDを取得
		dockingID_ = ImGui::GetID(kConsoleName_.c_str());

		//* scene
		DisplayScene();
		DisplayGame();

		//* MonoBehavior
		DisplayOutliner();
		DisplayAttribute();

		//* System
		DisplayLog();
		DisplaySystem();

		//* Performance
		DisplayPerformance();

	}
}

void BetaConsole::Draw() {

	{
		Sxavenger::BeginOffscreen(localRenderTexture_.get());

		if (selectedBehavior_.has_value()) {
			auto behavior = (*selectedBehavior_.value());
			pipeline_.SetPipeline(BetaConsolePipeline::kConsoleLocal);

			auto commandList = Sxavenger::GetCommandList();

			for (uint32_t i = 0; i < behavior->GetIA()->GetMeshSize(); ++i) {
				behavior->GetIA()->SetBuffers(i);

				commandList->SetGraphicsRootConstantBufferView(0, localCamera_->GetGPUVirtualAddress());
				commandList->SetGraphicsRootDescriptorTable(1, behavior->GetIA()->GetTextureHandle(i));

				behavior->GetIA()->DrawCall(i);
			}
		}

		Sxavenger::EndOffscreen(localRenderTexture_.get());
	}
	
	{
		Sxavenger::BeginOffscreen(sceneRenderTexture_.get());

		for (auto& behavior : behaviors_) {
			
			pipeline_.SetPipeline(BetaConsolePipeline::kDefault_Diffuse);

			auto commandList = Sxavenger::GetCommandList();

			for (uint32_t i = 0; i < behavior->GetIA()->GetMeshSize(); ++i) {
				behavior->GetIA()->SetBuffers(i);

				commandList->SetGraphicsRootConstantBufferView(0, sceneCamera_->GetGPUVirtualAddress());
				commandList->SetGraphicsRootConstantBufferView(1, behavior->GetTransform().GetGPUVirtualAddress());
				commandList->SetGraphicsRootDescriptorTable(2, behavior->GetIA()->GetTextureHandle(i));

				behavior->GetIA()->DrawCall(i);
			}
		}

		Sxavenger::EndOffscreen(sceneRenderTexture_.get());
	}

	{
		Sxavenger::BeginOffscreen(gameRenderTexture_.get());

		for (auto& behavior : behaviors_) {

			pipeline_.SetPipeline(BetaConsolePipeline::kDefault_Diffuse);

			auto commandList = Sxavenger::GetCommandList();

			for (uint32_t i = 0; i < behavior->GetIA()->GetMeshSize(); ++i) {
				behavior->GetIA()->SetBuffers(i);

				commandList->SetGraphicsRootConstantBufferView(0, gameCamera_->GetGPUVirtualAddress());
				commandList->SetGraphicsRootConstantBufferView(1, behavior->GetTransform().GetGPUVirtualAddress());
				commandList->SetGraphicsRootDescriptorTable(2, behavior->GetIA()->GetTextureHandle(i));

				behavior->GetIA()->DrawCall(i);
			}
		}

		Sxavenger::EndOffscreen(gameRenderTexture_.get());
	}

}

void BetaConsole::DockingConsole() const {
	ImGui::SetNextWindowDockID(dockingID_, ImGuiCond_FirstUseEver);
}

void BetaConsole::SetMonoBehavior(MonoBehavior* behavior) {
	behaviors_.emplace_back(behavior);
}

void BetaConsole::RemoveMonoBehavior(MonoBehavior* behavior) {
	if (selectedBehavior_.has_value()) {
		if (behavior == (*selectedBehavior_.value())) {
			selectedBehavior_ = std::nullopt;
			// todo: prevに移動
		}
	}

	behaviors_.remove(behavior);
}

void BetaConsole::Log(const std::string& log, const Color4f& color) {
	// logの追加
	logDatas_.emplace_front(log, color);

	while (logDatas_.size() >= kMaxLog_) {
		logDatas_.pop_back(); //!< 一番古いログの削除
	}
}

BetaConsole* BetaConsole::GetInstance() {
	static BetaConsole instance;
	return &instance;
}

void BetaConsole::DisplayMainMenu() {
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("Config")) {
		//* console config
		ImGui::SeparatorText("console config");

		ImGui::Checkbox("Display Console",     &isDisplayConsole_);
		ImGui::Checkbox("Output ImGui Layout", &isOutputImGuiIni_);

		//* Window(ImGui) config *//
		ImGui::SeparatorText("window config");

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

void BetaConsole::DisplayGame() {
	DockingConsole();
	static bool isOpenWindow = true;
	ImGui::Begin("Game ## Beta", &isOpenWindow, windowFlags_);

	DisplayTextureImGuiFullWindow(gameRenderTexture_.get());

	ImGui::End();
}

void BetaConsole::DisplayScene() {
	DockingConsole();
	static bool isOpenWindow = true;

	ImGui::Begin("Scene ## Beta", &isOpenWindow, windowFlags_);

	DisplayTextureImGuiFullWindow(sceneRenderTexture_.get());

	if (ImGui::IsWindowFocused()) {
		sceneCamera_->Update();
	}

	ImGui::End();
}

void BetaConsole::DisplayOutliner() {
	DockingConsole();
	static bool isOpenWindow = true;
	ImGui::Begin("Outliner ## Beta", &isOpenWindow, windowFlags_);

	for (auto behaviorIt = behaviors_.begin(); behaviorIt != behaviors_.end(); ++behaviorIt) {
		SelectableMonoBehavior(behaviorIt);
	}

	ImGui::End();
}

void BetaConsole::DisplayAttribute() {
	DockingConsole();
	static bool isOpenWindow = true;
	ImGui::Begin("Attribute ## Beta", &isOpenWindow, windowFlags_);

	if (selectedBehavior_.has_value()) {
		auto behavior = (*selectedBehavior_.value());

		ImGui::SeparatorText(behavior->GetName().c_str());

		behavior->MonoBehaviorImGui();    //!< systemのimgui
		ImGui::Spacing();
		behavior->SetMonoBehaviorImGui(); //!< userのimgui

		if (ImGui::IsWindowFocused()) {
			localCamera_->Update();
		}
	}

	ImGui::End();
}

void BetaConsole::DisplayLog() {
	DockingConsole();
	static bool isOpenWindow = true;
	ImGui::Begin("Console ## Beta", &isOpenWindow, windowFlags_);

	ImVec2 cntRegionMax = ImGui::GetWindowContentRegionMax();
	ImVec2 cntRegionMin = ImGui::GetWindowContentRegionMin();
	ImVec2 wndSize = { cntRegionMax.x - cntRegionMin.x, cntRegionMax.y - cntRegionMin.y };

	ImGui::BeginChild(ImGui::GetID((void*)0), wndSize, ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysVerticalScrollbar);

	for (auto& log : logDatas_) {
		ImGui::TextColored(
			{ log.color.r, log.color.g, log.color.b, log.color.a },
			log.log.c_str()
		);
	}

	ImGui::EndChild();

	if (ImGui::Button("clear")) {
		logDatas_.clear();
	}

	ImGui::End();
}

void BetaConsole::DisplaySystem() {
	DockingConsole();
	static bool isOpenWindow = true;
	ImGui::Begin("System ## Beta", &isOpenWindow, windowFlags_);

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

		if (updateLimit_.has_value()) { //!< updateLimitがある場合
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
						ImGui::Text("[LStick] x: %d, y: %d", gamepad->GetLStick().x, gamepad->GetLStick().y);

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

		if (ImGui::TreeNode("CBV_SRV_UAV")) {

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

void BetaConsole::DisplayPerformance() {
	// systemの状態によってstyleを変える
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 defaultWindowColor = style.Colors[ImGuiCol_WindowBg]; //!< 後で元に戻すので保存

	if (isUpdateRequired_) {
		// windowの色を変更
		style.Colors[ImGuiCol_WindowBg] = ImGuiManager::ToImVec4({ 45, 5, 8, 255 });
	}

	DockingConsole();
	static bool isOpenWindow = true;
	ImGui::Begin("Performance ## Beta", &isOpenWindow, windowFlags_ | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

	DeltaTimePoint framesPerSec = Performance::GetDeltaTime(SecondsUnit::s);

	ImGui::Text("exec speed / frame: %.6f", framesPerSec.time);
	ImGui::SameLine();
	ImGui::Text("FPS: %.0f", 1.0f / framesPerSec.time);

	ImGui::End();

	style.Colors[ImGuiCol_WindowBg] = defaultWindowColor;
}

void BetaConsole::InitConfig() {
	windowFlags_ = 0;
	windowFlags_ |= ImGuiWindowFlags_NoCollapse;
	windowFlags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	windowFlags_ |= ImGuiWindowFlags_NoMove;
	windowFlags_ |= ImGuiWindowFlags_NoResize;
}

void BetaConsole::InitRenderTarget() {
	gameRenderTexture_ = std::make_unique<RenderTexture>();
	gameRenderTexture_->Create(Sxavenger::GetDxCommon(), kWindowSize);

	gameCamera_ = std::make_unique<Camera3D>();
	gameCamera_->Init();

	sceneRenderTexture_ = std::make_unique<RenderTexture>();
	sceneRenderTexture_->Create(Sxavenger::GetDxCommon(), kWindowSize);

	sceneCamera_ = std::make_unique<DebugCamera3D>();
	sceneCamera_->Init();

	localRenderTexture_ = std::make_unique<RenderTexture>();
	localRenderTexture_->Create(Sxavenger::GetDxCommon(), kWindowSize, ToColor4f(0x303030FF)); //!< 固定サイズにしてもいいかも

	localCamera_ = std::make_unique<DebugCamera3D>();
	localCamera_->Init();
}

void BetaConsole::TermRenderTarget() {
	gameRenderTexture_.reset();
	gameCamera_.reset();

	sceneRenderTexture_.reset();
	sceneCamera_.reset();

	localRenderTexture_.reset();
	localCamera_.reset();
}

void BetaConsole::DisplayTextureImGuiFullWindow(const BaseTexture* texture) {
	
	// タブ等を排除した全体のwindowSize計算
	ImVec2 regionMax  = ImGui::GetWindowContentRegionMax();
	ImVec2 regionMin  = ImGui::GetWindowContentRegionMin();
	ImVec2 windowSize = { regionMax.x - regionMin.x, regionMax.y - regionMin.y };

	Vector2f textureSize = texture->GetSize();
	
	// 画像アス比と分割したWindowアス比の計算
	float textureAspectRatio = textureSize.x / textureSize.y;
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
	ImGui::Image((ImTextureID)texture->GetGPUHandleSRV().ptr, displayTextureSize);

}

bool BetaConsole::IsSelectedBehavior(MonoBehavior* behavior) {
	if (selectedBehavior_.has_value()) {
		return behavior == (*selectedBehavior_.value());
	}

	return false;
}

void BetaConsole::SelectableMonoBehavior(const std::list<MonoBehavior*>::iterator& behaviorIt) {
	
	// iteratorの中身の取得
	auto behavior = (*behaviorIt);

	// 選択されているかの確認
	bool isSelected = IsSelectedBehavior(behavior);

	// selectableに表示される名前の設定
	std::string label = behavior->GetName() + std::format("##{:p}", reinterpret_cast<void*>(behavior)); //!< 名前重複対策

	// TODO: Node解決
	if (ImGui::Selectable(label.c_str(), isSelected)) {
		selectedBehavior_ = behaviorIt;
		localCamera_->Reset();
	}
}