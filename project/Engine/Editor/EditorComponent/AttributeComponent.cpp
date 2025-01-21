#include "AttributeComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/OutlinerEditor.h>

//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AttributeComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AttributeComponent::SetToOutliner() {
	Assert(CheckIteratorEmpty(), "iterator already registered.");

	// outlinerに登録
	sEditorEngine->ExecuteEditorFunction<OutlinerEditor>([this](OutlinerEditor* editor) {
		iterator_ =  editor->SetAttribute(this);
	});
}

void AttributeComponent::SetChild(AttributeComponent* child) {
	Assert(child->CheckIteratorEmpty(), "child component iterator already registered.");
	child->parent_   = this;
	child->iterator_ = children_.emplace(children_.end(), child);
}

void AttributeComponent::RemoveIterator() {
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
		// childから削除
		parent_->children_.erase(iterator_.value());
	}

	parent_   = nullptr;
	iterator_ = std::nullopt;
}

AttributeComponent::Status AttributeComponent::GetStatus() const {
	if (iterator_.has_value()) {
		if (parent_ == nullptr) {
			return Status::Outliner;

		} else {
			return Status::Child;
		}
	}

	return Status::Unregistered;
}

bool AttributeComponent::CheckIteratorEmpty() const {
	return GetStatus() == Status::Unregistered;
}
