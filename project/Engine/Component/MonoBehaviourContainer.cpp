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

MonoBehaviourContainer* MonoBehaviourContainer::GetInstance() {
	static MonoBehaviourContainer instance;
	return &instance;
}
