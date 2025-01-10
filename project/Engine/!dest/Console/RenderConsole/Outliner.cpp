#include "Outliner.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* console
#include "../Console.h"

//=========================================================================================
// static variables
//=========================================================================================

const ImVec4 Outliner::kBehaviorDisableColor_ = { 0.4f, 0.4f, 0.4f, 1.0f };

////////////////////////////////////////////////////////////////////////////////////////////
// Outliner class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Outliner::Init(Console* console) {
	//!< 引数の保存
	console_ = console;
}

void Outliner::Term() {
}

BehaviorIterator Outliner::SetBehavior(BaseBehavior* behavior) {
	return outliner_.emplace(outliner_.end(), behavior);
}

void Outliner::EraseBehavior(const BehaviorIterator& iterator) {
	RemoveAttributeBehavior(iterator);
	outliner_.erase(iterator);
}

void Outliner::ResetBehavior() {
	RemoveUniqueBehavior();
	ResetAttributeBehavior();
	outliner_.clear();
	// FIXME: ここでoutlinerのclearを行うと, behavior側のiteratorが無効になる
}

void Outliner::RemoveAttributeBehavior(const BehaviorIterator& iterator) {
	if (attributeIterator_.has_value() && *attributeIterator_.value() == *iterator) {
		if (attributeIterator_.value() == attributeTable_->begin()) {
			ResetAttributeBehavior();

		} else {
			attributeIterator_.value()--;
		}
	}
}

void Outliner::ResetAttributeBehavior() {
	attributeIterator_ = std::nullopt;
	attributeTable_ = nullptr;
}

void Outliner::RemoveUniqueBehavior() {
	behaviors_.clear();
}

void Outliner::UpdateUniqueRemove() {
	behaviors_.remove_if([](const std::unique_ptr<RenderBehavior>& behavior) {
		return behavior->IsDelete();
	});
}

void Outliner::DisplayOutliner() {
	console_->DockingConsole();
	ImGui::Begin("Outliner ## Outliner", nullptr, console_->GetWindowFlag());

	SelectableBehavior(outliner_);

	ImGui::End();
}

void Outliner::DisplayAttribute() {
	console_->DockingConsole();
	ImGui::Begin("Attribute ## Outliner", nullptr, console_->GetWindowFlag());

	if (attributeIterator_.has_value()) {
		auto behavior = (*attributeIterator_.value());

		ImGui::SeparatorText(behavior->GetName().c_str());

		//!< base imgui
		behavior->BaseAttributeImGui();
		ImGui::Separator();
		ImGui::Spacing();

		//!< system imgui
		behavior->SystemAttributeImGui();
		ImGui::Spacing();

		//!< user imgui
		behavior->SetAttributeImGui();

		if (ImGui::IsWindowFocused()) {
			//localCamera_->Update();
		}
	}

	ImGui::End();
}

void Outliner::DrawSystematicBehavior(SxavGraphicsFrame* frame, const BehaviorContainer& container) {
	for (const auto& behavior : container) {
		if (behavior->IsActive()) {
			if (behavior->CheckRenderingFlag(BehaviorRenderingType::kSystematic)) {
				behavior->DrawSystematic(frame);
			}

			if (!behavior->GetChildren().empty()) {
				DrawSystematicBehavior(frame, behavior->GetChildren());
			}
		}
	}
}

void Outliner::DrawAdaptiveBehavior(SxavGraphicsFrame* frame, const BehaviorContainer& container) {
	for (const auto& behavior : container) {

		if (behavior->IsActive()) {
			if (behavior->CheckRenderingFlag(BehaviorRenderingType::kAdaptive)) {
				behavior->DrawAdaptive(frame);
			}
			if (!behavior->GetChildren().empty()) {
				DrawAdaptiveBehavior(frame, behavior->GetChildren());
			}
		}
	}
}

void Outliner::DrawLateAdaptiveBehavior(SxavGraphicsFrame* frame, const BehaviorContainer& container) {
	for (const auto& behavior : container) {
		if (behavior->IsActive()) {
			if (behavior->CheckRenderingFlag(BehaviorRenderingType::kLateAdaptive)) {
				behavior->DrawLateAdaptive(frame);
			}
			if (!behavior->GetChildren().empty()) {
				DrawLateAdaptiveBehavior(frame, behavior->GetChildren());
			}
		}
	}
}

bool Outliner::IsSelectedBehavior(BaseBehavior* behavior) {
	if (attributeIterator_.has_value()) {
		return behavior == (*attributeIterator_.value());
	}

	return false;
}

void Outliner::SelectableBehavior(const BehaviorContainer& container) {
	for (auto itr = container.begin(); itr != container.end(); ++itr) {

		BaseBehavior* behavior = (*itr);
		bool isSelected        = IsSelectedBehavior(behavior);

		std::string label = behavior->GetName() + std::format("##{:p}", reinterpret_cast<void*>(behavior)); //!< 名前重複対策

		// 非activeの場合, 灰色に
		if (!behavior->IsActive()) {
			ImGui::PushStyleColor(ImGuiCol_Text, kBehaviorDisableColor_);
		}

		if (behavior->GetChildren().empty()) { //!< 子がいない場合
			if (ImGui::Selectable(label.c_str(), isSelected)) {
				attributeIterator_ = itr;
				attributeTable_ = &container;
				//localCamera_->Reset();
			}

		} else {
			ImGuiTreeNodeFlags flags
				= ImGuiTreeNodeFlags_OpenOnDoubleClick
				| ImGuiTreeNodeFlags_OpenOnArrow;

			if (isSelected) {
				flags |= ImGuiTreeNodeFlags_Selected;
			}

			bool isOpenTreeNode = ImGui::TreeNodeEx(label.c_str(), flags);

			if (ImGui::IsItemClicked()) {
				attributeIterator_ = itr;
				attributeTable_ = &container;
				//localCamera_->Reset();
			}

			if (isOpenTreeNode) {
				SelectableBehavior(behavior->GetChildren());
				ImGui::TreePop();
			}
		}

		if (!behavior->IsActive()) {
			ImGui::PopStyleColor();
		}
	}
}
