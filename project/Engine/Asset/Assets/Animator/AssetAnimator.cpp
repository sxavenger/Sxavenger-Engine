#include "AssetAnimator.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Asset Animator class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetAnimator::Load(_MAYBE_UNUSED const DirectXThreadContext* context) {

	uint32_t option = 0;

	if (param_.has_value()) {
		option = std::any_cast<uint32_t>(param_);
	}

	Animator::Load(BaseAsset::GetFilepath(), option);
}
