#include "ALightActor.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ALightActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

const ALightActor::RayQueryShadow& ALightActor::GetShadow() const {
	Assert(shadow_ != nullptr, "shadow buffer is not create.");
	return shadow_->At(0);
}

ALightActor::RayQueryShadow& ALightActor::GetShadow() {
	Assert(shadow_ != nullptr, "shadow buffer is not create.");
	return shadow_->At(0);
}
