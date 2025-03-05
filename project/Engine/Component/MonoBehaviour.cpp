#include "MonoBehaviour.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <execution>

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehaviour class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MonoBehaviour::Term() {
	// parentの登録解除
	if (HasParent()) {
		parent_->RemoveChild(this);
	}

	struct Visit {
		void operator()(MonoBehaviour* behavior) const {
			behavior->Term();
		}

		void operator()(std::unique_ptr<MonoBehaviour>& behavior) const {
			auto tmp = std::move(behavior);
			operator()(tmp.get());
		}
	};

	// childrenの登録解除
	while (!children_.empty()) {
		std::visit(Visit{}, children_.back());
	}

	// componentの登録解除
	std::for_each(/*std::execution::seq, */components_.begin(), components_.end(), [](const std::pair<std::type_index, ComponentStorage::ComponentIterator>& pair) {
		const auto& [type, iterator] = pair;
		sComponentStorage->UnregisterComponent(type, iterator);
	});
	components_.clear();
}

void MonoBehaviour::SetParent(MonoBehaviour* parent) {
	Assert(!HasParent(), "behaviour already have parent.");
	parent_   = parent;
	iterator_ = parent->AddHierarchy(this);
}

void MonoBehaviour::AddChild(MonoBehaviour* child) {
	Assert(!child->HasParent(), "behaviour already have parent.");
	child->parent_   = this;
	child->iterator_ = AddHierarchy(child);
}

void MonoBehaviour::AddChild(std::unique_ptr<MonoBehaviour> child) {
	auto ptr = child.get();
	Assert(!ptr->HasParent(), "behaviour already have parent.");
	ptr->parent_   = this;
	ptr->iterator_ = AddHierarchy(std::move(child));
}

void MonoBehaviour::RemoveChild(MonoBehaviour* child) {
	RemoveHierarchy(child->iterator_.value());
	child->parent_   = nullptr;
	child->iterator_ = std::nullopt;
}

MonoBehaviour::Iterator MonoBehaviour::AddHierarchy(std::variant<MonoBehaviour*, std::unique_ptr<MonoBehaviour>>&& child) {
	return children_.emplace(children_.end(), std::move(child));
}

void MonoBehaviour::RemoveHierarchy(const Iterator& iterator) {
	children_.erase(iterator);
}
