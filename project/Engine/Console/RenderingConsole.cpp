#include "RenderingConsole.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* forward
#include <Engine/Game/Behavior/BaseBehavior.h>

//* console
#include "SystemConsole.h"

//* engine
#include <Engine/Game/SxavengerGame.h>

//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderingConsole class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderingConsole::Init() {
	renderingPipeline_ = std::make_unique<RenderingPipeline>();
	renderingPipeline_->Init();
}

void RenderingConsole::Term() {
	renderingPipeline_.reset();
}

void RenderingConsole::UpdateConsole() {
	if (isDisplayRenderingConsole_) {
		DisplayOutliner();
		DisplayAttribute();
	}
}

void RenderingConsole::RenderSystematic(SxavengerFrame* frame) {
	frame->BeginSystematic(true, true);

	for (auto behavior : behaviors_) {
		DrawSystematicBehavior(behavior, frame);
	}

	frame->EndSystematic();
}

void RenderingConsole::RenderAdaptive(SxavengerFrame* frame) {
	frame->BeginAdaptive();

	for (auto behavior : behaviors_) {
		DrawAdaptiveBehavior(behavior, frame);
	}

	SxavengerGame::DrawToScene(frame->GetCamera());
	frame->EndAdaptive();
}

void RenderingConsole::SetBehavior(BaseBehavior* behavior) {
	behaviors_.emplace_back(behavior);
}

void RenderingConsole::RemoveSelectedBehavior(BaseBehavior* behavior) {
	// behaviorがselectされている状態だった場合, 選択の解除
	if (selectedBehavior_.has_value() && behavior == (*selectedBehavior_.value())) {
		
		if (selectedBehavior_.value() == selectedBehaviorTable_->begin()) { //!< tableの先頭だった場合
			// 先頭がないのでnulloptとなる
			selectedBehavior_      = std::nullopt;
			selectedBehaviorTable_ = nullptr;

		} else {
			// 先頭に移動
			selectedBehavior_.value()--;
		}

	}
}

void RenderingConsole::RemoveBehavior(BaseBehavior* behavior) {
	RemoveSelectedBehavior(behavior);
	behaviors_.remove(behavior);
}

void RenderingConsole::SetRenderingPipeline(RenderingPipelineType type) const {
	renderingPipeline_->SetPipeline(type);
}

void RenderingConsole::DisplayOutliner() {
	sSystemConsole->DockingConsole();
	ImGui::Begin("Outliner ## Rendering Console", nullptr, sSystemConsole->GetWindowFlag());

	for (auto behaviorIt = behaviors_.begin(); behaviorIt != behaviors_.end(); ++behaviorIt) {
		SelectableBehavior(behaviorIt, behaviors_);
	}

	ImGui::End();
}

void RenderingConsole::DisplayAttribute() {
	sSystemConsole->DockingConsole();
	ImGui::Begin("Attribute ## Rendering Console", nullptr, sSystemConsole->GetWindowFlag() | ImGuiWindowFlags_AlwaysVerticalScrollbar);

	if (selectedBehavior_.has_value()) {
		auto behavior = (*selectedBehavior_.value());

		ImGui::SeparatorText(behavior->GetName().c_str());

		behavior->SystemAttributeImGui();
		ImGui::Spacing();
		behavior->SetAttributeImGui();

		if (ImGui::IsWindowFocused()) {
			//localCamera_->Update();
		}
	}

	ImGui::End();
}

bool RenderingConsole::IsSelectedBehavior(BaseBehavior* behavior) {
	if (selectedBehavior_.has_value()) {
		return behavior == (*selectedBehavior_.value());
	}

	return false;
}

void RenderingConsole::SelectableBehavior(const BehaviorContainer::const_iterator& it, const BehaviorContainer& table) {
	// todo: list内どうしの要素入れ替え

	// iteratorの中身の取得
	auto behavior = (*it);

	// 選択されているかの確認
	bool isSelected = IsSelectedBehavior(behavior);

	// selectableに表示される名前の設定
	std::string label = behavior->GetName() + std::format("##{:p}", reinterpret_cast<void*>(behavior)); //!< 名前重複対策

	if (behavior->GetChildren().empty()) { //!< 子がいない場合
		if (ImGui::Selectable(label.c_str(), isSelected)) {
			selectedBehavior_      = it;
			selectedBehaviorTable_ = &table; 

			//localCamera_->Reset();
		}

	} else { //!< 子がいる場合

		ImGuiTreeNodeFlags flags
			= ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_OpenOnArrow;

		if (isSelected) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		if (ImGui::TreeNodeEx(label.c_str(), flags)) {
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) { //!< selectされた場合
				selectedBehavior_      = it;
				selectedBehaviorTable_ = &table;

				//localCamera_->Reset();
			}

			for (auto childIt = behavior->GetChildren().begin(); childIt != behavior->GetChildren().end(); ++childIt) {
				SelectableBehavior(childIt, behavior->GetChildren());
			}

			ImGui::TreePop();
		}
	}
}

void RenderingConsole::DrawSystematicBehavior(BaseBehavior* behavior, SxavengerFrame* frame) {
	behavior->DrawSystematic(frame->GetCamera());

	for (auto child : behavior->GetChildren()) {
		DrawSystematicBehavior(child, frame);
	}
}

void RenderingConsole::DrawAdaptiveBehavior(BaseBehavior* behavior, SxavengerFrame* frame) {
	behavior->DrawAdaptive(frame->GetCamera());

	for (auto child : behavior->GetChildren()) {
		DrawAdaptiveBehavior(child, frame);
	}
}
