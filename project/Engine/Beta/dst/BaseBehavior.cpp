#include "BaseBehavior.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* Console
#include <Engine/Beta/BetaConsole.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseBehavior base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseBehavior::Term() {
	Assert(children_.empty()); //!< 親(this)を消す場合, 子をdeleteまたはclearしてなければならない

	// selectから削除
	sBetaConsole->RemoveSelectedBehavior(this);

	if (parent_ == nullptr) { //!< 親がいない場合
		// console tableから削除
		sBetaConsole->RemoveBehavior(this);

	} else { //!< 親がいる場合
		// 親から自分を削除
		parent_->RemoveChild(this);
		parent_ = nullptr;
	}
}

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
