#include "SystemConsole.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* console
#include "../Console.h"

//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SystemConsole class
////////////////////////////////////////////////////////////////////////////////////////////

void SystemConsole::Init(Console* console) {
	console_ = console;
}

void SystemConsole::Term() {
}

void SystemConsole::UpdateConsole() {
	if (isDisplaySystemConsole_) {
		DisplayLog();
	}
}

void SystemConsole::Log(const std::string& label, const std::string& detail, const std::optional<Color4f> color) {
	while (logs_.size() >= limitLog_) {
		logs_.pop_front();
	}

	if (!logs_.empty()) {
		auto& back = logs_.back();

		//!< 同じログが連続している場合
		if (label == back.label && detail == back.detail && back.color == color) {
			back.duplication++;
			return;
		}
	}

	LogEntry& log = logs_.emplace_back();
	log.label  = label;
	log.detail = detail;
	log.color  = color;
}

void SystemConsole::ShowSystemMenu() {
	if (ImGui::BeginMenu("Process")) {
		MenuDummy();

		ImGui::SeparatorText("process frame");

		if (updateLimit_ == std::nullopt) { //!< 更新処理の制限がない場合
			if (ImGui::Button("frame stop")) {
				updateLimit_ = 0; //!< 制限の代入
			}

		} else {
			if (ImGui::Button("frame start")) {
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

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("window collection")) {
		MenuDummy();

		auto windowCollection = SxavengerSystem::GetGameWindowCollection();
		windowCollection->SystemDebugGui();

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("DescriptorHeaps")) {
		MenuDummy();

		auto desciptorHeaps = SxavengerSystem::GetDxDescriptorHeaps();
		desciptorHeaps->SystemDebugGui();

		ImGui::EndMenu();
	}
}

void SystemConsole::DisplayLog() {
	console_->DockingConsole();
	ImGui::Begin("Log ## System Console", nullptr, console_->GetWindowFlag());

	ImVec2 regionMax = ImGui::GetWindowContentRegionMax();
	ImVec2 regionMin = ImGui::GetWindowContentRegionMin();
	ImVec2 windowSize = { regionMax.x - regionMin.x, regionMax.y - regionMin.y };

	{
		ImGui::BeginChild(ImGui::GetID((void*)0), windowSize, ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

		// logの表示
		for (const auto& log : logs_) {
			if (log.color.has_value()) {

				Color4f color = log.color.value();

				if (log.duplication != 0) {
					ImGui::TextColored(
						{ color.r, color.g, color.b, color.a },
						std::format("{} [x{}]", log.label, log.duplication + 1).c_str()
					);

				} else {
					ImGui::TextColored(
						{ color.r, color.g, color.b, color.a },
						log.label.c_str()
					);
				}

			} else {
				if (log.duplication != 0) {
					ImGui::Text(
						std::format("{} [x{}]", log.label, log.duplication + 1).c_str()
					);

				} else {
					ImGui::Text(
						log.label.c_str()
					);
				}
			}

			if (ImGui::IsItemHovered() && !log.detail.empty()) {
				if (ImGui::BeginTooltip()) {
					ImGui::Text(log.detail.c_str());
					ImGui::EndTooltip();
				}
			}
		}

		ImGui::EndChild();
	}

	//* setting *//

	if (ImGui::Button("claer log")) {
		logs_.clear();
	}

	ImGui::SliderInt("limit log", (int*)&limitLog_, 0, 128);
	ImGui::End();
}

void SystemConsole::MenuDummy() {
	static const ImVec2 size = { 240.0f, 0.0f };
	ImGui::Dummy(size);
}

