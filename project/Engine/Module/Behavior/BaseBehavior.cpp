#include "BaseBehavior.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseBehavior::SetChild(BaseBehavior* child) {
	Assert(child->iterator_ == std::nullopt, "child iterator is not nullopt.");
	child->parent_   = this;
	child->iterator_ = children_.emplace(children_.end(), child);
}

void BaseBehavior::RemoveChild(BaseBehavior* child) {
	if (child->iterator_.has_value() && std::holds_alternative<BehaviorNode::const_iterator>(child->iterator_.value())) {
		//!< 自身のiteratorから削除
		const BehaviorNode::const_iterator& iterator = std::get<BehaviorNode::const_iterator>(child->iterator_.value());
		children_.erase(iterator);

		//!< 削除したのでiteratorをnulloptに代入
		child->iterator_ = std::nullopt;
	}
}
