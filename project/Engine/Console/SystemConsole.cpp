#include "SystemConsole.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>
#include <Engine/System/Performance.h>
#include <Engine/Game/SxavengerGame.h>


//=========================================================================================
// SystemConsole // static variables
//=========================================================================================

const Color4f SystemConsole::kCommentOutColor = { 0.0f, 0.55f, 0.0f, 1.0f };
const Color4f SystemConsole::kErrorColor      = { 0.8f, 0.0f, 0.0f, 1.0f };
const Color4f SystemConsole::kWarningColor    = { 0.8f, 0.8f, 0.0f, 1.0f };

const std::string SystemConsole::kConsoleName_      = "Sxavenger Engine Console";
const std::string SystemConsole::kImGuiIniFilepath_ = "imgui.ini";

////////////////////////////////////////////////////////////////////////////////////////////
// SystemConsole class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SystemConsole::Init() {
	InitImGuiConfig();
	InitConsole();
	InitFrame();

	Sxavenger::LoadTexture("resources/checker_black.png");
}

void SystemConsole::Term() {
	TermFrame();
	TermConsole();
}

void SystemConsole::UpdateConsole() {
	UpdateKeyAction();

	UpdateConsoleShortcut();

	DisplayMainMenu();

	if (isDisplayConsole_) {

		//!< docking用IDを取得
		dockingId_ = ImGui::GetID(kConsoleName_.c_str());

		DisplayPerformance();
		DisplayLog();
		DisplayGame();
		DisplayScene();
		
		RenderingConsole::UpdateConsole();
		ProcessConsole::UpdateConsole();
	}

}

void SystemConsole::Draw() {
	
	RenderingConsole::RenderSystematic(gameFrame_.get());
	RenderingConsole::RenderSystematic(sceneFrame_.get());

	gameFrame_->TransitionSystematicToXclipse();
	sceneFrame_->TransitionSystematicToXclipse();

	Sxavenger::TranstionAllocator();

	ProcessConsole::ProcessXclipse(gameFrame_.get());
	ProcessConsole::ProcessXclipse(sceneFrame_.get());


	if (RenderingConsole::isRaytracingEnabled_) {
		RenderingConsole::SetupRaytracing();
		RenderingConsole::RenderRaytracing(sceneFrame_.get()); //!< test
		// HACK: raytracingが1frame分しか使えない
	}
	

	gameFrame_->TransitionXclipseToAdaptive();
	sceneFrame_->TransitionXclipseToAdaptive();

	Sxavenger::TranstionAllocator();

	RenderingConsole::RenderAdaptive(gameFrame_.get());
	RenderingConsole::RenderAdaptive(sceneFrame_.get());

	gameFrame_->TransitionAdaptiveToVisual();
	sceneFrame_->TransitionAdaptiveToVisual();

	Sxavenger::TranstionAllocator();

	ProcessConsole::ProcessVisual(gameFrame_.get());
	ProcessConsole::ProcessVisual(sceneFrame_.get());

	gameFrame_->TransitionVisualToAdaptive();
	sceneFrame_->TransitionVisualToAdaptive();

	Sxavenger::TranstionAllocator();

	RenderingConsole::RenderAdaptive(gameFrame_.get());

	{
		sceneFrame_->BeginAdaptive();

		// 視錐台描画
		gameCamera_->DrawFrustum(ToColor4f(0xFAFA00FF), 8.0f);

		// colliderの描画
		SxavengerGame::DrawColliders();

		SxavengerGame::DrawToScene(sceneCamera_.get());

		sceneFrame_->EndAdaptive();
	}

	Sxavenger::TranstionAllocator();
}

void SystemConsole::DockingConsole() const {
	ImGui::SetNextWindowDockID(dockingId_, ImGuiCond_FirstUseEver);
}

void SystemConsole::Log(const std::string& log, const std::optional<Color4f>& color) {
	// logの追加
	logs_.emplace_front(log, color);

	while (logs_.size() >= limitLog_) {
		logs_.pop_back(); //!< 一番古いログの削除
	}
}

void SystemConsole::PresentToScreen() {
	if (presentType_ == PresentToScreenType::kPresent_Game) {
		gameFrame_->PresentAdaptiveToScreen();

	} else if (presentType_ == PresentToScreenType::kPresent_Scene) {
		sceneFrame_->PresentAdaptiveToScreen();
	}
}

SystemConsole* const SystemConsole::GetInstance() {
	static SystemConsole instance;
	return &instance;
}

void SystemConsole::InitImGuiConfig() {
	//* IO
	ImGuiIO& io    = ImGui::GetIO();
	io.IniFilename = NULL; //!< imguiの書き込み, 読み込みを手動に変更
	ImGui::LoadIniSettingsFromDisk(kImGuiIniFilepath_.c_str());

	//* windowFlag
	windowFlag_ = 0;
	windowFlag_ |= ImGuiWindowFlags_NoCollapse;
	windowFlag_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	windowFlag_ |= ImGuiWindowFlags_NoMove;
	windowFlag_ |= ImGuiWindowFlags_NoResize;
}

void SystemConsole::InitConsole() {
	RenderingConsole::Init();
	ProcessConsole::Init();
}

void SystemConsole::TermConsole() {
	RenderingConsole::Term();
	ProcessConsole::Term();
}

void SystemConsole::InitFrame() {
	gameCamera_ = std::make_unique<CineCamera>();
	gameCamera_->SetToConsole("game camera");

	gameFrame_ = std::make_unique<SxavengerFrame>();
	gameFrame_->Create(kWindowSize);
	gameFrame_->SetCamera(gameCamera_.get());

	sceneCamera_ = std::make_unique<DebugCamera3D>();
	sceneCamera_->SetToConsole("scene camera");

	sceneFrame_ = std::make_unique<SxavengerFrame>();
	sceneFrame_->Create(kWindowSize);
	sceneFrame_->SetCamera(sceneCamera_.get());
}

void SystemConsole::TermFrame() {
	gameFrame_.reset();
	gameCamera_.reset();
	sceneFrame_.reset();
	sceneCamera_.reset();
}


void SystemConsole::UpdateConsoleShortcut() {

	if ((Sxavenger::IsPressKey(DIK_LALT) || Sxavenger::IsPressKey(DIK_RALT))
		&& Sxavenger::IsTriggerKey(DIK_F)) {
		isDisplayConsole_ = !isDisplayConsole_;
	}
}

void SystemConsole::DisplayMainMenu() {
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("Config")) {
		ImGui::Dummy({ 200.0f, 0.0f });

		//* console config
		ImGui::SeparatorText("console config");
		ImGui::Checkbox("display console", &isDisplayConsole_);

		ImGui::SameLine();
		ImGui::Dummy({ 8.0f, 0.0f });
		ImGui::SameLine();
		ImGui::TextDisabled("ALT+F");

		{
			ImGui::SeparatorText("present to screen type");

			uint32_t current = static_cast<uint32_t>(presentType_);

			if (ImGui::RadioButton("none", current == static_cast<uint32_t>(PresentToScreenType::kPresent_None))) {
				presentType_ = PresentToScreenType::kPresent_None;
			}

			ImGui::SameLine();

			if (ImGui::RadioButton("game frame", current == static_cast<uint32_t>(PresentToScreenType::kPresent_Game))) {
				presentType_ = PresentToScreenType::kPresent_Game;
			}

			ImGui::SameLine();

			if (ImGui::RadioButton("scene frame", current == static_cast<uint32_t>(PresentToScreenType::kPresent_Scene))) {
				presentType_ = PresentToScreenType::kPresent_Scene;
			}
		}

		//* imgui config
		ImGui::SeparatorText("imgui config");

		// .ini layout
		if (ImGui::Button("output layout")) {
			ImGui::SaveIniSettingsToDisk(kImGuiIniFilepath_.c_str());
		}

		// windowflag
		ImGuiWindowFlags lockWindow = 0;
		lockWindow |= ImGuiWindowFlags_NoMove;
		lockWindow |= ImGuiWindowFlags_NoResize;
		ImGui::CheckboxFlags("lock console imgui", &windowFlag_, lockWindow);

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Layout")) {
		ImGui::Dummy({ 200.0f, 0.0f });
		//!< ImGui公式のstyleに関するeditor
		ImGui::ShowStyleEditor();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("System")) {
		ImGui::Dummy({ 200.0f, 0.0f });
		DisplaySystemMenu();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Window")) {
		ImGui::Dummy({ 200.0f, 0.0f });
		ImGui::Checkbox("display rendering console", &(RenderingConsole::isDisplayRenderingConsole_));
		ImGui::Checkbox("display process console",   &(ProcessConsole::isDisplayProcessConsole_));
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Rendering")) {
		ImGui::Checkbox("raytracing enabled", &(RenderingConsole::isRaytracingEnabled_));
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}

void SystemConsole::DisplayPerformance() {
	// systemの状態によってstyleを変える
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 defaultWindowColor = style.Colors[ImGuiCol_WindowBg]; //!< 後で元に戻すので保存

	if (isUpdateRequired_) {
		// windowの色を変更
		style.Colors[ImGuiCol_WindowBg] = ImGuiManager::ToImVec4({ 45, 5, 8, 255 });
	}

	DockingConsole();
	ImGui::Begin("Performance ## System Console", nullptr, windowFlag_ | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

	DeltaTimePoint framesPerSec = Performance::GetDeltaTime(SecondsUnit::s);

	ImGui::Text("exec speed / frame: %.6f", framesPerSec.time);
	ImGui::SameLine();
	ImGui::Text("FPS: %.0f", 1.0f / framesPerSec.time);

	ImGui::End();

	style.Colors[ImGuiCol_WindowBg] = defaultWindowColor;
}

void SystemConsole::DisplayLog() {
	DockingConsole();
	ImGui::Begin("Log ## System Console", nullptr, windowFlag_);

	ImVec2 cntRegionMax = ImGui::GetWindowContentRegionMax();
	ImVec2 cntRegionMin = ImGui::GetWindowContentRegionMin();
	ImVec2 wndSize = { cntRegionMax.x - cntRegionMin.x, cntRegionMax.y - cntRegionMin.y };

	ImGui::BeginChild(ImGui::GetID((void*)0), wndSize, ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

	for (const auto& log : logs_) {
		if (log.second.has_value()) {
			ImGui::TextColored(
				{ log.second.value().r, log.second.value().g, log.second.value().b, log.second.value().a },
				log.first.c_str()
			);

		} else {
			ImGui::Text(
				log.first.c_str()
			);
		}
	}

	ImGui::EndChild();

	//* setting *//

	if (ImGui::Button("claer log")) {
		logs_.clear();
	}

	ImGui::SliderInt("limit log", (int*)&limitLog_, 0, 128);

	ImGui::End();
}

void SystemConsole::DisplayGame() {
	DockingConsole();
	ImGui::Begin("Game ## System Console", nullptr, windowFlag_ | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	DisplayTextureImGuiFullWindow(Sxavenger::GetTexture<BaseTexture>("resources/checker_black.png")); //< HACK
	DisplayTextureImGuiFullWindow(gameFrame_->GetAdaptive()->GetTexture());

	ImGui::End();
}

void SystemConsole::DisplayScene() {
	DockingConsole();
	ImGui::Begin("Scene ## System Console", nullptr, windowFlag_ | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	DisplayTextureImGuiFullWindow(Sxavenger::GetTexture<BaseTexture>("resources/checker_black.png")); //< HACK
	DisplayTextureImGuiFullWindow(sceneFrame_->GetAdaptive()->GetTexture());

	if (ImGui::IsWindowFocused()) {
		sceneCamera_->Update();
	}

	ImGui::End();
}

void SystemConsole::DisplaySystemMenu() {
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
			static const uint32_t step = 1;
			static uint32_t count = 0;
			ImGui::InputScalar("update count", ImGuiDataType_U32, &count, &step);

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

}

void SystemConsole::UpdateKeyAction() {

	//* alt(left || right) + f: フルスクリーンモード切り替え
	if ((Sxavenger::IsPressKey(DIK_LALT) || Sxavenger::IsPressKey(DIK_RALT))
		&& Sxavenger::IsTriggerKey(DIK_F)) {
		isDisplayConsole_ = !isDisplayConsole_;
	}
}

void SystemConsole::DisplayTextureImGuiFullWindow(const MultiViewTexture* texture) const {
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
	ImGui::Image(texture->GetGPUHandleSRV().ptr, displayTextureSize);
}

void SystemConsole::DisplayTextureImGuiFullWindow(const BaseTexture* texture, const ImVec4& boarderColor) const {
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
	ImGui::Image(texture->GetGPUHandleSRV().ptr, displayTextureSize, { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, boarderColor);
}