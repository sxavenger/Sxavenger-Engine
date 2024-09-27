#include "BaseBehavior.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* Console
#include <Engine/Beta/BetaConsole.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseBehavior base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseBehavior::SetToConsole() {
	sBetaConsole->SetBehavior(this);
}

void BaseBehavior::SetToConsole(const std::string& name) {
	SetName(name);
	SetToConsole();
}

void BaseBehavior::SetChild(BaseBehavior* child) {
	child->parent_ = this;
	children_.emplace_back(child);
}

void BaseBehavior::RemoveChild(BaseBehavior* child) {
	children_.remove(child);
}

void BaseBehavior::Term() {
	if (parent_ == nullptr) { //!< 親がいない場合, consoleから削除
		Assert(children_.empty()); //!< 親が消される場合, 子を先にdeleteする
		sBetaConsole->RemoveBehavior(this);

	} else { //!< 親がいる場合, 親から自分を削除
		parent_->RemoveChild(this);
	}
}
