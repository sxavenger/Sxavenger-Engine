#include "BaseAsset.h"

bool BaseAsset::CheckAndBeginLoad() {
	std::unique_lock<std::mutex> lock(mutex_);
	
	if (state_ == AssetState::Unloaded) {
		state_ = AssetState::Loading;
		return true;
	}

	return false;
}
