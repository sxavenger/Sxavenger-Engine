#include "SystemConsole.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* console
#include "Console.h"

//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SystemConsole class
////////////////////////////////////////////////////////////////////////////////////////////

void SystemConsole::Init() {
}

void SystemConsole::Term() {
}

void SystemConsole::UpdateConsole() {
	if (isDisplaySystemConsole_) {
		DisplayLog();
	}
}

void SystemConsole::Log(const std::string& log, const std::optional<Color4f>& color) {
	// logの追加
	logs_.emplace_front(log, color);

	while (logs_.size() >= limitLog_) {
		logs_.pop_back(); //!< 一番古いログの削除
	}
}

void SystemConsole::DisplaySystemMenu() {
	if (ImGui::BeginMenu("Process")) {
		ImGui::Dummy({ 200.0f, 0.0f });

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

	if (ImGui::BeginMenu("thread")) {
		ImGui::Dummy({ 200.0f, 0.0f });

		auto threadCollection = SxavengerSystem::GetThreadCollection();
		threadCollection->SystemDebugGui();

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("DescriptorHeaps")) {
		ImGui::Dummy({ 200.0f, 0.0f });

		auto desciptorHeaps = SxavengerSystem::GetDxDescriptorHeaps();
		desciptorHeaps->SystemDebugGui();

		ImGui::EndMenu();
	}


	if (ImGui::BeginMenu("CompileBlobColleciton")) {
		ImGui::Dummy({ 200.0f, 0.0f });

		auto compileBlobCollection = SxavengerSystem::GetDxCompileBlobCollection();
		compileBlobCollection->SystemDebugGui();

		ImGui::EndMenu();
	}
}

void SystemConsole::DisplayLog() {
	sConsole->DockingConsole();
	ImGui::Begin("Log ## System Console", nullptr, sConsole->GetWindowFlag());

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

