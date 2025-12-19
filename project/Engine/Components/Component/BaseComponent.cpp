#include "BaseComponent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

MonoBehaviour* BaseComponent::GetBehaviour() const {
	return behaviour_;
}

bool BaseComponent::IsActive() const {
	return behaviour_->IsActive();
}

MonoBehaviour* BaseComponent::GetParent() const {
	return behaviour_->GetParent().Get();
}

MonoBehaviour* BaseComponent::RequireParent() const {
	StreamLogger::AssertA(behaviour_->HasParent(), "parent is nullptr.");
	return behaviour_->GetParent().Get();
}
