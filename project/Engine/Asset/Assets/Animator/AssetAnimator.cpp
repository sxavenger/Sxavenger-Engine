#include "AssetAnimator.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Asset Animator class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetAnimator::Load(_MAYBE_UNUSED const DirectXThreadContext* context) {
	if (BaseAsset::IsLoaded()) {
		return;
	}

	BaseAsset::SetState(BaseAsset::State::Loading);

	Animator::Load(filepath_, assimpOption_);
	context->ExecuteAllAllocators();

	BaseAsset::SetState(BaseAsset::State::Complete);
}
