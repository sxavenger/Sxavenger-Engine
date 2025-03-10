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

void Attribute::SetToOutliner() {
	Assert(CheckIteratorEmpty(), "iterator already registered.");

	// outlinerに登録
	sEditorEngine->ExecuteEditorFunction<OutlinerEditor>([this](OutlinerEditor* editor) {
		iterator_ = editor->SetAttribute(this);
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
	}

	iterator_ = std::nullopt;
}

Attribute::Status Attribute::GetStatus() const {
	if (iterator_.has_value()) {
		return Status::Outliner;
	}

	return Status::Unregistered;
}

bool Attribute::CheckIteratorEmpty() const {
	return GetStatus() == Status::Unregistered;
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

		if (ImGui::Selectable(label.c_str(), isSelect)) {
			selected_ = attribute;
		}
	}
}
