#include "DevelopEditor.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/Utility/RuntimeLogger.h>
#include <Engine/System/System.h>

//* external
#include <imgui.h>

//* c++
#include <ranges>

////////////////////////////////////////////////////////////////////////////////////////////
// DevelopEditor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DevelopEditor::Init() {
}

void DevelopEditor::ShowMainMenu() {
	if (ImGui::BeginMenu("develop")) {
		MenuPadding();

		ImGui::SeparatorText("develop");
		ShowConfigMenu();
		ShowProcessMenu();
		ShowSystemMenu();
		ShowThreadMenu();
		ShowInputMenu();

		ImGui::EndMenu();
	}
}

void DevelopEditor::ShowWindow() {
	ShowPerformanceWindow();
	ShowConsole();
}

void DevelopEditor::BreakPoint(const std::source_location& location) {
	isProcessRequired_ = false;
	processLimit_      = 0;

	std::string locate = "";
	locate += "[function]: ";
	locate += location.function_name();
	locate += "\n";

	locate += "[line]: ";
	locate += std::to_string(location.line());
	locate += "\n";

	RuntimeLogger::LogComment("[DevelopEditor]" , "break point called. \n locate: " + locate);
}

void DevelopEditor::ShowConfigMenu() {
	if (ImGui::BeginMenu("config")) {
		MenuPadding();
		ImGui::SeparatorText("config");

		//* device
		ImGui::Text(std::format("enable debug layer: {}",          Configuration::GetConfig().enableDebugLayer).c_str());
		ImGui::Text(std::format("enable GPU based validation: {}", Configuration::GetConfig().enableGPUBasedValidation).c_str());

		//* tearing
		ImGui::BeginDisabled(!Configuration::GetSupport().isSupportTearing);
		ImGui::Checkbox("tearing allow", &Configuration::FixConfig().isTearingAllowed);

		if (!Configuration::GetSupport().isSupportTearing) {
			if (Configuration::GetConfig().isTearingAllowed) {
				ImGui::Text("tearing is not support.");

			} else {
				ImGui::Text("tearing is not true in config.");
			}
		}

		ImGui::EndDisabled();

		//* frame rate lock
		ImGui::Checkbox("lock frame rate", &Configuration::FixConfig().isLockFrameRate);

		ImGui::BeginDisabled(!Configuration::GetConfig().isLockFrameRate);
		ImGui::InputFloat("target frame rate", &Configuration::FixConfig().targetFrameRate);
		ImGui::EndDisabled();

		ImGui::EndMenu();
	}
}

void DevelopEditor::ShowProcessMenu() {
	if (ImGui::BeginMenu("process")) {
		MenuPadding();
		ImGui::SeparatorText("process");

		if (processLimit_.has_value()) {
			if (ImGui::Button("resume")) { //!< 再生
				processLimit_      = std::nullopt;
				isProcessRequired_ = true;
			}

			ImGui::Spacing();

			if (ImGui::Button("step frame")) { //!< 1フレーム進む
				processLimit_ = 1;
			}

			// 何フレーム更新するかの決定処理
			static const uint32_t step = 1;
			static uint32_t count      = 0;
			ImGui::InputScalar("update count", ImGuiDataType_U32, &count, &step);

			ImGui::SameLine();

			if (ImGui::Button("execute")) {
				processLimit_ = count;
				count = 0;
			}

			if (processLimit_.has_value()) {
				ImGui::Text("update count remaining : %u <frame>", processLimit_.value());
			}

		} else {
			if (ImGui::Button("stop")) { //!< 停止
				BreakPoint();
			}
		}

		ImGui::EndMenu();
	}

	//* process update *//

	isProcessRequired_ = true;

	if (processLimit_.has_value()) { //!< updateLimitがある場合
		if (processLimit_.value() == 0) { //!< 更新処理の回数が0の時
			isProcessRequired_ = false; //!< 更新処理のstop

		} else {
			processLimit_.value() -= 1;
		}
	}
}

void DevelopEditor::ShowSystemMenu() {
	if (ImGui::BeginMenu("system")) {
		MenuPadding();
		ImGui::SeparatorText("system");

		if (ImGui::BeginMenu("descriptor")) {
			MenuPadding();
			System::GetDxDescriptorHeaps()->SystemDebugGui();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("window")) {
			MenuPadding();
			System::GetWindowCollection()->SystemDebugGui();
			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}

void DevelopEditor::ShowThreadMenu() {
	if (ImGui::BeginMenu("thread")) {
		MenuPadding();
		System::GetAsyncThreadCollection()->SystemDebugGui();
		ImGui::EndMenu();
	}
}

void DevelopEditor::ShowInputMenu() {
	if (ImGui::BeginMenu("input")) {
		MenuPadding();
		System::GetInput()->SystemDebugGui();
		ImGui::EndMenu();
	}
}

void DevelopEditor::ShowPerformanceWindow() {
	if (isProcessRequired_) { //!< 更新中の場合
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGuiController::ToImVec4({ 45, 5, 8, 255 }));
	}

	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Performance ## Engine Developer Editor", nullptr, BaseEditor::GetWindowFlag() | ImGuiWindowFlags_NoTitleBar);

	TimePointf<TimeUnit::second> time = System::GetDeltaTimef();

	std::string text = "";
	text += std::format("[exec speed / frame]: {:.6f}", time.time) + " ";
	text += std::format("[frame per second]: {:.1f}",   1.0f / time.time);
	ImGui::Text(text.c_str());

	if (ImGui::BeginItemTooltip()) {

		ImGui::SeparatorText("CPU Timestamp");

		if (ImGui::BeginTable("## record cpu timestamp", 2, ImGuiTableFlags_Borders)) {
			ImGui::TableSetupColumn("name");
			ImGui::TableSetupColumn("timestamp - delta / [elapsed]");
			ImGui::TableHeadersRow();

			const TimestampCpu::Timestamp& timestamp = System::GetTimestampCpu()->GetTimestamp();

			for (const auto& stamp : timestamp) {
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text(stamp.name.c_str());

				ImGui::TableNextColumn();
				ImGui::Text(std::format("{:.2f}ms / [{:.2f}ms]", stamp.delta.time, stamp.elapsed.time).c_str());
			}

			ImGui::EndTable();
		}

		ImGui::SeparatorText("GPU Timestamp");

		if (ImGui::BeginTable("## record gpu timestamp", 2, ImGuiTableFlags_Borders)) {
			ImGui::TableSetupColumn("name");
			ImGui::TableSetupColumn("timestamp - section");
			ImGui::TableHeadersRow();

			const TimestampGpu::Timestamp& timestamp = System::GetTimestampGpu()->GetTimestamp();

			for (const auto& stamp : timestamp.stamps) {
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text(stamp.name.c_str());

				ImGui::TableNextColumn();
				ImGui::Text(std::format("{:.2f}ms", stamp.section.time).c_str());
			}
			ImGui::EndTable();
		}

		ImGui::EndTooltip();
	}

	ImGui::End();

	if (isProcessRequired_) {
		ImGui::PopStyleColor();
	}
}

void DevelopEditor::ShowConsole() {
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Console ## Engine Developer Editor", nullptr, BaseEditor::GetWindowFlag());

	//* console option *//

	static std::pair<const char*, ImColor> kStyles[magic_enum::enum_count<RuntimeLogger::Type>()] = {
		{ "-", ImGui::GetStyle().Colors[ImGuiCol_Text] },
		{ "●", ImVec4(0.80f, 0.80f, 0.80f, 1.0f) },
		{ "!", ImVec4(1.00f, 0.80f, 0.20f, 1.0f) },
		{ "x", ImVec4(1.00f, 0.30f, 0.30f, 1.0f) },
	};

	if (ImGui::BeginTable("## console table", 2, ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | ImGuiTableFlags_BordersH)) {

		ImGui::TableSetupColumn("Log");
		ImGui::TableHeadersRow();

		for (const auto& log : RuntimeLogger::GetLogs()) {

			const auto& style = kStyles[static_cast<uint32_t>(log.type)];

			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			ImGui::PushID(&log);

			// 行全体を Selectable にする
			ImGui::Selectable(
				"##log_row",
				false,
				ImGuiSelectableFlags_SpanAllColumns
			);

			ImGui::SameLine();

			// アイコン
			ImGui::TextColored(style.second, "%s", style.first);
			ImGui::SameLine();

			// メインログテキスト
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(style.second));
			ImGui::TextWrapped("%s", log.label.c_str());
			ImGui::PopStyleColor();

			// 補助情報（薄く）
			ImGui::SameLine(0.0f, 12.0f);
			ImGui::PushStyleColor(
				ImGuiCol_Text,
				ImVec4(0.55f, 0.55f, 0.55f, 1.0f)
			);
			ImGui::Text(
				"[%s] x%u",
				log.category.c_str(),
				log.count
			);
			ImGui::PopStyleColor();

			ImGui::PopID();
		}

		ImGui::EndTable();
	}

	ImGui::End();
}
