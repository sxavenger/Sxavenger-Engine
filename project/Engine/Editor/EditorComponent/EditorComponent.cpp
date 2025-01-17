#include "EditorComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EditorComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EditorComponent::Term() {
	Assert(children_.empty(), "component remain children."); //!< 親(this)を消す場合, 子をclearしてなければならない
	EraseToIteratorContainer();
}

void EditorComponent::SetChild(EditorComponent* child) {
	child->parent_   = this;
	child->SetIterator(children_.emplace(children_.end(), child));
}

void EditorComponent::SetToConsole() {
}

void EditorComponent::SetIterator(const Iterator& iterator) {
	Assert(!iterator_.has_value(), "component is already set to container.");
	iterator_ = iterator;
}

void EditorComponent::EraseToIteratorContainer() {
	if (!iterator_.has_value()) {
		return;
	}

	if (parent_ != nullptr) { //!< 親に所属している場合
		parent_->children_.erase(iterator_.value());
		parent_ = nullptr;

	} else { //!< Editorに所属している場合
		// todo:

	}

	iterator_ = std::nullopt;
}
