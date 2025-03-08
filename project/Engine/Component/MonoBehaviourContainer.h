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

	//* singleton *//

	static MonoBehaviourContainer* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* container
	MonoBehaviour::Container container_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehaviourContainer class instance
////////////////////////////////////////////////////////////////////////////////////////////
static MonoBehaviourContainer* sMonoBehaviourContainer = MonoBehaviourContainer::GetInstance();
