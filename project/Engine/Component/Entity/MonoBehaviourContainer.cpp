#include "MonoBehaviourContainer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehaviourContainer class methods
////////////////////////////////////////////////////////////////////////////////////////////

MonoBehaviour::ContainerIterator MonoBehaviourContainer::AddBehaviour(MonoBehaviour* behaviour) {
	return container_.emplace(container_.end(), behaviour);
}

void MonoBehaviourContainer::RemoveBehaviour(const MonoBehaviour::ContainerIterator& iterator) {
	container_.erase(iterator);
}

void MonoBehaviourContainer::ForEach(const std::function<void(MonoBehaviour*)>& function) {
	//!< 計算量: o(MonoBehaviourの数) container_.size()ではないので注意.
	for (auto& behaviour : container_) {
		Execute(behaviour, function);
	}
}

MonoBehaviourContainer* MonoBehaviourContainer::GetInstance() {
	static MonoBehaviourContainer instance;
	return &instance;
}

void MonoBehaviourContainer::Execute(MonoBehaviour* behaviour, const std::function<void(MonoBehaviour*)>& function) {
	function(behaviour);

	for (auto& child : behaviour->GetChildren()) {
		static MonoBehaviour::GetPtrVisitor visitor;
		Execute(std::visit(visitor, child), function);
	}
}
