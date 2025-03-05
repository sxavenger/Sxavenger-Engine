#include "DevelopEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DevelopEditor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DevelopEditor::Init() {
}

void DevelopEditor::ShowMainMenu() {
	if (ImGui::BeginMenu("develop")) {
		MenuPadding();

		ImGui::SeparatorText("develop");
		ShowProcessMenu();
		ShowSystemMenu();
		ShowThreadMenu();

		ImGui::EndMenu();
	}
}

void DevelopEditor::ShowWindow() {
	ShowPerformanceWindow();
}

void DevelopEditor::ShowProcessMenu() {
	if (ImGui::BeginMenu("process")) {
		MenuPadding();
		ImGui::SeparatorText("process");

		if (processLimit_.has_value()) {
			if (ImGui::Button("resame")) { //!< 再生
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

			auto descriptorHeaps = SxavengerSystem::GetDxDescriptorHeaps();
			descriptorHeaps->SystemDebugGui();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("window")) {

			auto windowCollection = SxavengerSystem::GetGameWindowCollection();
			windowCollection->SystemDebugGui();

			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}
}

void DevelopEditor::ShowThreadMenu() {
	if (ImGui::BeginMenu("thread")) {
		MenuPadding();

		auto collection = SxavengerAsset::GetCollection();
		collection->SystemDebugGui();

		ImGui::EndMenu();
	}
}

void DevelopEditor::ShowPerformanceWindow() {
	if (isProcessRequired_) { //!< 更新中の場合
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGuiController::ToImVec4({ 45, 5, 8, 255 }));
	}

	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Performace ## Engine Developer Editor", nullptr, BaseEditor::GetWindowFlag() | ImGuiWindowFlags_NoTitleBar);

	const Performance* performance = SxavengerSystem::GetPerformance();
	TimePointf<TimeUnit::second> time = performance->GetDeltaTime();

	std::string text = "";
	text += std::format("[exec speed / frame]: {:.6f}", time.time) + " ";
	text += std::format("[frame per second]: {:.1f}",   1.0f / time.time);
	ImGui::Text(text.c_str());

	if (ImGui::BeginItemTooltip()) {

		if (ImGui::BeginTable("## record laps", 2, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders)) {
			ImGui::TableSetupColumn("name");
			ImGui::TableSetupColumn("lap");
			ImGui::TableHeadersRow();

			const auto& laps = performance->GetLap();
			for (auto itr = laps.begin(); itr != laps.end(); ++itr) {
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text(itr->first.c_str());

				ImGui::TableNextColumn();

				if (itr == laps.begin()) {
					ImGui::Text(std::format("{:.2f}ms", itr->second.time).c_str());

				} else {
					auto prev = std::prev(itr);
					ImGui::Text(std::format("{:.2f}ms", itr->second.time - prev->second.time).c_str());
				}
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

void DevelopEditor::BreakPoint() {
	isProcessRequired_ = false;
	processLimit_      = 0;
}
