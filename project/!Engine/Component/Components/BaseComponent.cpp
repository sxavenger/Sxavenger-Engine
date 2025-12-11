#include "BaseComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Entity/MonoBehaviour.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

MonoBehaviour* BaseComponent::GetBehaviour() const {
	return behaviour_;
}

bool BaseComponent::IsActive() const {
	return behaviour_->IsActive();
}
