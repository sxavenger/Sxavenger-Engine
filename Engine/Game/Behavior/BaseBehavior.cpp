#include "BaseBehavior.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* console
#include <Engine/Console/SystemConsole.h>

//* engine
#include <Engine/System/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseBehavior base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseBehavior::SetToConsole() {
	sSystemConsole->SetBehavior(this);
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
	Assert(children_.empty(), "behavior children is not empty."); //!< 親(this)を消す場合, 子をdeleteまたはclearしてなければならない

	// selectからの削除
	sSystemConsole->RemoveSelectedBehavior(this);

	if (parent_ == nullptr) { //!< root(親がいない)場合
		// consoleから削除
		sSystemConsole->RemoveBehavior(this);

	} else { //!< 親がいる場合
		// 親から自分を削除
		parent_->RemoveChild(this);
		parent_ = nullptr;
	}
}