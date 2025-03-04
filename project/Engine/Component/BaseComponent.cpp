#include "BaseComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

MonoBehaviour* BaseComponent::GetBehaviour() const {
	Assert(behaviour_ != nullptr, "behaviour is nullptr.");
	return behaviour_;
}
