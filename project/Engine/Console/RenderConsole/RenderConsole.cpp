#include "RenderConsole.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* console
#include "../Console.h"

////////////////////////////////////////////////////////////////////////////////////////////
// RenderConsole class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderConsole::Init(Console* console) {
	console_ = console;
}

void RenderConsole::Term() {
	ResetBehavior();
}

void RenderConsole::UpdateConsole() {
	if (isDisplayRenderConsole_) {
		//* behavior *//
		UpdateUniqueRemove();

		//* window *//
		DisplayOutliner();
		DisplayAttribute();
	}
}

BehaviorIterator RenderConsole::SetBehavior(BaseBehavior* behavior) {
	return outliner_.emplace(outliner_.end(), behavior);
}

void RenderConsole::EraseBehavior(const BehaviorIterator& iterator) {
	RemoveAttributeBehavior(iterator);
	outliner_.erase(iterator);
}

void RenderConsole::ResetBehavior() {
	RemoveUniqueBehavior();
	ResetAttributeBehavior();
	outliner_.clear();
}

void RenderConsole::RemoveAttributeBehavior(const BehaviorIterator& iterator) {
	if (attributeIterator_.has_value() && *attributeIterator_.value() == *iterator) {
		if (attributeIterator_.value() == attributeTable_->begin()) {
			ResetAttributeBehavior();

		} else {
			attributeIterator_.value()--;
		}
	}
}

void RenderConsole::ResetAttributeBehavior() {
	attributeIterator_ = std::nullopt;
	attributeTable_    = nullptr;
}

void RenderConsole::RemoveUniqueBehavior() {
	behaviors_.clear();
}

void RenderConsole::ShowRenderConsoleMenu() {
	if (ImGui::BeginMenu("behavior outliner")) {
		MenuDummy();

		ImGui::SeparatorText("outliner");
		if (ImGui::Button("remove unique behavior")) {
			RemoveUniqueBehavior();
		}

		ImGui::EndMenu();
	}
}

void RenderConsole::ShowBehaviorMenu() {
	if (ImGui::BeginMenu("Add")) {
		MenuDummy();

		if (ImGui::Selectable("base behavior", false)) {
			auto& behavior = behaviors_.emplace_back();
			behavior = std::make_unique<RenderBehavior>();
		}

		ImGui::EndMenu();
	}
}

void RenderConsole::DisplayOutliner() {
	console_->DockingConsole();
	ImGui::Begin("Outliner ## Render Console", nullptr, console_->GetWindowFlag());

	SelectableBehavior(outliner_);

	ImGui::End();
}

void RenderConsole::DisplayAttribute() {
	console_->DockingConsole();
	ImGui::Begin("Attribute ## Render Console", nullptr, console_->GetWindowFlag());

	if (attributeIterator_.has_value()) {
		auto behavior = (*attributeIterator_.value());

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

bool RenderConsole::IsSelectedBehavior(BaseBehavior* behavior) {
	if (attributeIterator_.has_value()) {
		return behavior == (*attributeIterator_.value());
	}

	return false;
}

void RenderConsole::SelectableBehavior(const BehaviorContainer& container) {

	for (auto itr = container.begin(); itr != container.end(); ++itr) {
		BaseBehavior* behavior = (*itr);
		bool isSelected        = IsSelectedBehavior(behavior);

		std::string label = behavior->GetName() + std::format("##{:p}", reinterpret_cast<void*>(behavior)); //!< 名前重複対策

		if (behavior->GetChildren().empty()) { //!< 子がいない場合
			if (ImGui::Selectable(label.c_str(), isSelected)) {
				attributeIterator_ = itr;
				attributeTable_    = &container;
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

			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) { //!< selectされた場合
				attributeIterator_ = itr;
				attributeTable_    = &container;
				//localCamera_->Reset();
			}

			if (isOpenTreeNode) {
				SelectableBehavior(behavior->GetChildren());
				ImGui::TreePop();
			}
		}
	}
}

void RenderConsole::UpdateUniqueRemove() {
	behaviors_.remove_if([](const std::unique_ptr<RenderBehavior>& behavior) {
		return behavior->IsDelete();
	});
}

void RenderConsole::MenuDummy() {
	static const ImVec2 size = { 240.0f, 0.0f };
	ImGui::Dummy(size);
}
