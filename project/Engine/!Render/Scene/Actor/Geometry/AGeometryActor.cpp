#include "AGeometryActor.h"

////////////////////////////////////////////////////////////////////////////////////////////
// AGeometryActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

bool AGeometryActor::CheckVisibility(Transparency target) const {
	if (isActive_) {
		return transparency_ == target;
	}

	return false;
}

AGeometryActor::Iterator AGeometryActor::AddChild(AGeometryActor* child) {
	return children_.emplace(children_.end(), child);
}
