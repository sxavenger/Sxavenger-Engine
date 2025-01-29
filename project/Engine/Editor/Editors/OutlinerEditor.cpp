#include "OutlinerEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// OutlinerEditor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void OutlinerEditor::Init() {
}

void OutlinerEditor::ShowMainMenu() {
}

void OutlinerEditor::ShowWindow() {
	ShowOutlinerWindow();
	ShowAttributeWindow();
}

AttributeComponent::Iterator OutlinerEditor::SetAttribute(AttributeComponent* attribute) {
	return outliner_.emplace(outliner_.end(), attribute);
}

void OutlinerEditor::RemoveAttribute(const AttributeComponent::Iterator& iterator) {
	outliner_.erase(iterator);
}

void OutlinerEditor::Clear() {
	for (auto& attribute : outliner_) {
		attribute->RemoveIterator();
	}
}

void OutlinerEditor::CheckSelectClear(const AttributeComponent::Iterator& iterator) {
	if (CheckSelected(iterator)) {
		if (selectIterator_.value() == selectContainer_->begin()) {
			selectIterator_  = std::nullopt;
			selectContainer_ = nullptr;

		} else {
			selectIterator_.value()--;
		}
	}
}

void OutlinerEditor::ShowOutlinerWindow() {
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Outliner ## Outliner Editor", nullptr, BaseEditor::GetWindowFlag());

	Selectable(outliner_);

	ImGui::End();
}

void OutlinerEditor::ShowAttributeWindow() {
	BaseEditor::SetNextWindowDocking();
	ImGui::Begin("Attribute ## Outliner Editor", nullptr, BaseEditor::GetWindowFlag());

	if (selectIterator_.has_value()) {
		AttributeComponent* component = (*selectIterator_.value());

		ImGui::SeparatorText(component->GetName().c_str());

		//!< base imgui
		component->ExecuteAttribute();
		
	}

	ImGui::End();
}

bool OutlinerEditor::CheckSelected(const AttributeComponent::Iterator& iterator) const {
	if (selectIterator_.has_value()) {
		return (*iterator) == (*selectIterator_.value());
	}

	return false;
}

void OutlinerEditor::Selectable(const AttributeComponent::Container& container) {
	for (auto itr = container.begin(); itr != container.end(); ++itr) {

		bool isSelect                 = CheckSelected(itr);
		AttributeComponent* component = *itr;

		std::string label = std::format("{} ## {:p}", component->GetName(), reinterpret_cast<const void*>(component));

		if (component->GetChildren().empty()) {
			if (ImGui::Selectable(label.c_str(), isSelect)) {
				selectIterator_  = itr;
				selectContainer_ = &container;
			}

		} else {

			ImGuiTreeNodeFlags flags
				= ImGuiTreeNodeFlags_OpenOnDoubleClick
				| ImGuiTreeNodeFlags_OpenOnArrow;

			if (isSelect) {
				flags |= ImGuiTreeNodeFlags_Selected;
			}

			bool isOpen = ImGui::TreeNodeEx(label.c_str(), flags);

			if (ImGui::IsItemClicked()) {
				selectIterator_  = itr;
				selectContainer_ = &container;
				//localCamera_->Reset();
			}

			if (isOpen) {
				Selectable(component->GetChildren());
				ImGui::TreePop();
			}
		}
	}
}
