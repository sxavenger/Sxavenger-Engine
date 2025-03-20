#include "OutlinerEditor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>
#include <Engine/Editor/EditorEngine.h>

//* external
#include <imgui.h>

//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// Attribute class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Attribute::Term() {
	while (!children_.empty()) {
		children_.front()->RemoveIterator();
	}

	RemoveIterator();
}

void Attribute::SetToOutliner() {
	Assert(CheckIteratorEmpty(), "iterator already registered.");

	// outlinerに登録
	sEditorEngine->ExecuteEditorFunction<OutlinerEditor>([this](OutlinerEditor* editor) {
		iterator_ = editor->SetAttribute(this);
		parent_   = nullptr;
	});
}

void Attribute::RemoveIterator() {

	Status status = GetStatus();

	if (status == Status::Unregistered) {
		return;
	}

	// 選択解除
	sEditorEngine->ExecuteEditorFunction<OutlinerEditor>([this](OutlinerEditor* editor) {
		editor->CheckSelectClear(iterator_.value());
	});

	if (status == Status::Outliner) {
		// outlinerから削除
		sEditorEngine->ExecuteEditorFunction<OutlinerEditor>([this](OutlinerEditor* editor) {
			editor->RemoveAttribute(iterator_.value());
		});

	} else if (status == Status::Child) {
		parent_->EraseChild(iterator_.value());
	}

	parent_   = nullptr;
	iterator_ = std::nullopt;
}

void Attribute::SetParent(Attribute* parent) {
	RemoveIterator();
	parent_   = parent;
	iterator_ = parent->AddChildren(this);
}

void Attribute::SetChild(Attribute* child) {
	child->SetParent(this);
}

Attribute::Status Attribute::GetStatus() const {
	if (iterator_.has_value()) {
		return parent_ == nullptr ? Status::Outliner : Status::Child;
	}

	return Status::Unregistered;
}

bool Attribute::CheckIteratorEmpty() const {
	return GetStatus() == Status::Unregistered;
}

Attribute::Iterator Attribute::AddChildren(Attribute* child) {
	return children_.emplace(children_.end(), child);
}

void Attribute::EraseChild(const Iterator& iterator) {
	children_.erase(iterator);
}

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

Attribute::Iterator OutlinerEditor::SetAttribute(Attribute* attribute) {
	return outliner_.emplace(outliner_.end(), attribute);
}

void OutlinerEditor::RemoveAttribute(const Attribute::Iterator& iterator) {
	outliner_.erase(iterator);
}

//void OutlinerEditor::Clear() {
//	for (auto& attribute : outliner_) {
//		attribute->RemoveIterator();
//		// todo: attribute component
//	}
//}

void OutlinerEditor::CheckSelectClear(const Attribute::Iterator& iterator) {
	if (CheckSelected((*iterator))) {
		selected_ = nullptr;
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

	if (selected_ != nullptr) {

		ImGui::SeparatorText(selected_->GetName().c_str());

		//!< base imgui
		selected_->ExecuteAttribute();
	}

	ImGui::End();
}

bool OutlinerEditor::CheckSelected(const Attribute* attribute) const {
	return attribute == selected_;
}

void OutlinerEditor::Selectable(const Attribute::Container& container) {

	for (const auto& attribute : container) {
		bool isSelect     = CheckSelected(attribute);
		std::string label = std::format("{} ## {:p}", attribute->GetName(), static_cast<const void*>(attribute));

		if (attribute->GetChildren().empty()) {

			if (ImGui::Selectable(label.c_str(), isSelect)) {
				selected_ = attribute;
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
				selected_ = attribute;
			}

			if (isOpen) {

				Selectable(attribute->GetChildren());

				ImGui::TreePop();
			}

		}
		
	}
}
