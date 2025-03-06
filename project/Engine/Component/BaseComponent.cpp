#include "BaseComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "MonoBehaviour.h"

//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

MonoBehaviour* BaseComponent::GetBehaviour() const {
	Assert(behaviour_ != nullptr, "behaviour is nullptr.");
	return behaviour_;
}

bool BaseComponent::IsActive() const {
	return behaviour_->IsActive();
}
