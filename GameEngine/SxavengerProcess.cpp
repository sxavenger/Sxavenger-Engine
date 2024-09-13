#include "SxavengerProcess.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerProcess class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerProcess::Init() {
	for (int i = 0; i < 4; ++i) {
		layers_.emplace_back(i);
	}
}

void SxavengerProcess::Update() {
	ImGui::Begin("Sxavenger Process Layer");

	for (auto it = layers_.begin(); it != layers_.end(); ++it) {

		std::string label = std::format("layer number {} ## {}", *it, *it);

		ImGui::Selectable(label.c_str(), false);

		// ドラックの確認
		if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) {
			// 上方向のドラッグ
			if (ImGui::GetMouseDragDelta(0).y < 0.0f) {
				if (it != layers_.begin()) { //!< 先頭レイヤーではない場合
					auto prev = std::prev(it);
					std::swap(*it, *prev);
					ImGui::ResetMouseDragDelta();
				}

			} else if (ImGui::GetMouseDragDelta(0).y > 0.0f) {
				if (std::next(it) != layers_.end()) { //!< 末尾レイヤーではない場合
					auto next = std::next(it);
					std::swap(*it, *next);
					ImGui::ResetMouseDragDelta();
				}
			}
		}
	}

	ImGui::End();
}

SxavengerProcess* SxavengerProcess::GetInstance() {
	static SxavengerProcess instance;
	return &instance;
}
