#include "BaseBehavior.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>
#include <Engine/Console/Console.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseBehavior::Term() {
	Assert(children_.empty(), "behavior remain child."); //!< 親(this)を消す場合, 子をdeleteまたはclearしてなければならない
	ResetIterator();
}

void BaseBehavior::SetToConsole() {
	CheckIterator();
	iterator_ = sConsole->SetBehavior(this);
}

void BaseBehavior::SetToConsole(const std::string& name) {
	SetName(name);
	SetToConsole();
}

void BaseBehavior::SetChild(BaseBehavior* child) {
	child->CheckIterator();
	child->parent_   = this;
	child->iterator_ = children_.emplace(children_.end(), child);
}

void BaseBehavior::EraseChild(BaseBehavior* child) {
	if (child->iterator_.has_value() && child->parent_ == this) {
		//!< 自身のiteratorから削除
		children_.erase(child->iterator_.value());

		//!< 削除したのでiteratorをnulloptに代入
		child->iterator_ = std::nullopt;
	}
}

void BaseBehavior::ResetIterator() {
	if (iterator_.has_value()) {
		const auto& itr = iterator_.value();

		sConsole->RemoveAttributeBehavior(itr);

		if (parent_ == nullptr) { //!< consoleとして設定されている場合
			sConsole->EraseBehavior(itr);

		} else { //!< 子として設定されている場合
			parent_->children_.erase(itr);
			parent_ = nullptr;
		}

		iterator_ = std::nullopt;
	}
}

void BaseBehavior::BaseAttributeImGui() {
	ImGui::Checkbox("active", &isActive_);
}

void BaseBehavior::CheckIterator() {
	Assert(iterator_ == std::nullopt, "behavior iterator is not nullopt.");
}