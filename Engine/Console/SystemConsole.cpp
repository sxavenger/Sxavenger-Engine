#include "SystemConsole.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>
#include <Engine/System/Performance.h>
#include <Engine/Game/Behavior/BaseBehavior.h>
_DXOBJECT_USING

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

	Sxavenger::LoadTexture("resources/checker_white.png");
}

void SystemConsole::Term() {
	TermFrame();
	TermConsole();
}

void SystemConsole::UpdateConsole() {
	DisplayMainMenu();

	if (isDisplayConsole_) {

		//!< docking用IDを取得
		dockingId_ = ImGui::GetID(kConsoleName_.c_str());

		DisplayPerformance();
		DisplayLog();
		DisplayGame();
		DisplayScene();

		if (isDisplayRenderingConsole_) {
			renderingConsole_->UpdateConsole();
		}
	}
}

void SystemConsole::Draw() {
	renderingConsole_->RenderSystematic(gameFrame_.get());
	renderingConsole_->RenderSystematic(sceneFrame_.get());

	gameFrame_->TransitionSystematicToXclipse();
	sceneFrame_->TransitionSystematicToXclipse();

	Sxavenger::TranstionAllocator();

	processConsole_->ProcessXclipse(gameFrame_.get());
	processConsole_->ProcessXclipse(sceneFrame_.get());

	gameFrame_->TransitionXclipseToAdaptive();
	sceneFrame_->TransitionXclipseToAdaptive();

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
	renderingConsole_ = std::make_unique<RenderingConsole>();
	renderingConsole_->Init();

	processConsole_ = std::make_unique<ProcessConsole>();
	processConsole_->Init();
}

void SystemConsole::TermConsole() {
	renderingConsole_.reset();
	processConsole_.reset();
}

void SystemConsole::InitFrame() {
	gameCamera_ = std::make_unique<CineCamera>();

	gameFrame_ = std::make_unique<SxavengerFrame>();
	gameFrame_->Create(kWindowSize);
	gameFrame_->SetCamera(gameCamera_.get());

	sceneCamera_ = std::make_unique<DebugCamera3D>();

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


void SystemConsole::DisplayMainMenu() {
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("Config")) {
		//* console config
		ImGui::SeparatorText("console config");
		ImGui::Checkbox("display console", &isDisplayConsole_);

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
		//!< ImGui公式のstyleに関するeditor
		ImGui::ShowStyleEditor();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("System")) {
		DisplaySystemMenu();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Window")) {
		ImGui::Checkbox("display rendering console", &isDisplayRenderingConsole_);
		ImGui::Checkbox("display process console",   &isDisplayProcessConsole_);
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
	ImGui::Begin("Game ## System Console", nullptr, windowFlag_ | ImGuiWindowFlags_NoScrollbar);

	DisplayTextureImGuiFullWindow(Sxavenger::GetTexture<BaseTexture>("resources/checker_white.png"), { 0.2f, 0.2f, 0.2f, 1.0f }); //< HACK
	DisplayTextureImGuiFullWindow(gameFrame_->GetAdaptiveTexture());

	ImGui::End();
}

void SystemConsole::DisplayScene() {
	DockingConsole();
	ImGui::Begin("Scene ## System Console", nullptr, windowFlag_ | ImGuiWindowFlags_NoScrollbar);

	DisplayTextureImGuiFullWindow(Sxavenger::GetTexture<BaseTexture>("resources/checker_white.png"), { 0.2f, 0.2f, 0.2f, 1.0f }); //< HACK
	DisplayTextureImGuiFullWindow(sceneFrame_->GetAdaptiveTexture());

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
