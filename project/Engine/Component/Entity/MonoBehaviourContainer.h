#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "MonoBehaviour.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehaviourContainer class
////////////////////////////////////////////////////////////////////////////////////////////
class MonoBehaviourContainer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MonoBehaviour::ContainerIterator AddBehaviour(MonoBehaviour* behaviour);

	void RemoveBehaviour(const MonoBehaviour::ContainerIterator& iterator);

	const MonoBehaviour::Container& GetContainer() const { return container_; }

	void ForEach(const std::function<void(MonoBehaviour*)>& function);

	template <class _ExecutionPolicy>
	void ForEach(_ExecutionPolicy&& policy, const std::function<void(MonoBehaviour*)>& function);

	//* singleton *//

	static MonoBehaviourContainer* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* container
	MonoBehaviour::Container container_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Execute(MonoBehaviour* behaviour, const std::function<void(MonoBehaviour*)>& function);

};

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehaviourContainer class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class _ExecutionPolicy>
inline void MonoBehaviourContainer::ForEach(_ExecutionPolicy&& policy, const std::function<void(MonoBehaviour*)>& function) {
	std::for_each(policy, container_.begin(), container_.end(), [&](MonoBehaviour* behaviour) {
		Execute(behaviour, function);
	});
}

////////////////////////////////////////////////////////////////////////////////////////////
// singleton
////////////////////////////////////////////////////////////////////////////////////////////
static MonoBehaviourContainer* sMonoBehaviourContainer = MonoBehaviourContainer::GetInstance();
