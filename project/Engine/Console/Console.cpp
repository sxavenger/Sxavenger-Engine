#include "Console.h"

//=========================================================================================
// static variables
//=========================================================================================

const std::string Console::kConsoleName_         = "Sxavenger Engine Console";
const std::string Console::kImGuiLayoutFilepath_ = "imgui.ini";

////////////////////////////////////////////////////////////////////////////////////////////
// Console class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Console::Init() {
	ImGui::LoadIniSettingsFromDisk(kImGuiLayoutFilepath_.c_str());

	//* windowFlag
	windowFlag_ = 0;
	windowFlag_ |= ImGuiWindowFlags_NoCollapse;
	windowFlag_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	windowFlag_ |= ImGuiWindowFlags_NoMove;
	windowFlag_ |= ImGuiWindowFlags_NoResize;

	SystemConsole::Init();

	runtime_.Begin();
}

void Console::Term() {
	runtime_.End();

	SystemConsole::Term();
}

void Console::UpdateConsole() {
	DisplayMainMenu();

	if (isDisplayConsole_) {

		//!< docking用IDを取得
		dockingId_ = ImGui::GetID(kConsoleName_.c_str());

		ImGui::ShowDemoWindow();
		ImGui::ShowDebugLogWindow();

		DisplayPerformace();

		SystemConsole::UpdateConsole();
	}
}

void Console::Draw() {
}

void Console::DockingConsole() const {
	ImGui::SetNextWindowDockID(dockingId_, ImGuiCond_FirstUseEver);
}

Console* Console::GetInstance() {
	static Console instance;
	return &instance;
}

void Console::DisplayMainMenu() {
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("Config")) {
		ImGui::Dummy({ 200.0f, 0.0f }); //!< menu width dummy

		if (ImGui::BeginMenu("Console")) {
			ImGui::Dummy({ 200.0f, 0.0f }); //!< menu width dummy

			//* console config
			ImGui::SeparatorText("console");
			ImGui::Checkbox("display console", &isDisplayConsole_);

			//* imgui config
			ImGui::SeparatorText("imgui config");

			// windowflag
			ImGuiWindowFlags lockWindow = 0;
			lockWindow |= ImGuiWindowFlags_NoMove;
			lockWindow |= ImGuiWindowFlags_NoResize;
			ImGui::CheckboxFlags("lock console", &windowFlag_, lockWindow);

			// .ini layout
			if (ImGui::Button("output layout ini")) {
				ImGui::SaveIniSettingsToDisk(kImGuiLayoutFilepath_.c_str());
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Layout")) {
			ImGui::Dummy({ 200.0f, 0.0f });
			//!< ImGui公式のstyleに関するeditor
			ImGui::ShowStyleEditor();
			ImGui::EndMenu();
		}

		ImGui::Text("exec elapsed: %.0f", runtime_.GetElapsedTime<TimeUnit::s>().time);

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Window")) {
		ImGui::Dummy({ 200.0f, 0.0f });
		ImGui::Checkbox("display system console", &(SystemConsole::isDisplaySystemConsole_));
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("System")) {
		ImGui::Dummy({ 200.0f, 0.0f });
		SystemConsole::DisplaySystemMenu();
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}

void Console::DisplayPerformace() {
	// systemの状態によってstyleを変える
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 defaultWindowColor = style.Colors[ImGuiCol_WindowBg]; //!< 後で元に戻すので保存

	if (SystemConsole::isUpdateRequired_) {
		// windowの色を変更
		style.Colors[ImGuiCol_WindowBg] = ImGuiController::ToImVec4({ 45, 5, 8, 255 });
	}

	DockingConsole();
	ImGui::Begin("Performance ## System Console", nullptr, windowFlag_ | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

	DeltaTimePoint<TimeUnit::s> framesPerSec = Performance::GetDeltaTime<TimeUnit::s>();

	ImGui::Text("exec speed / frame: %.6f", framesPerSec.time);
	ImGui::SameLine();
	ImGui::Text("FPS: %.0f", 1.0f / framesPerSec.time);
	

	ImGui::End();

	style.Colors[ImGuiCol_WindowBg] = defaultWindowColor;
}
