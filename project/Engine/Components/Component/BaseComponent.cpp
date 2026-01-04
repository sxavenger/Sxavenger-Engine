#include "BaseComponent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "../Entity/EntityBehaviour.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

EntityBehaviour* BaseComponent::GetBehaviour() const {
	return behaviour_;
}

bool BaseComponent::IsActive() const {
	return behaviour_->IsActive();
}

EntityBehaviour* BaseComponent::GetParent() const {
	return behaviour_->GetParent().Get();
}

EntityBehaviour* BaseComponent::RequireParent() const {
	StreamLogger::AssertA(behaviour_->HasParent(), "parent is nullptr.");
	return behaviour_->GetParent().Get();
}
