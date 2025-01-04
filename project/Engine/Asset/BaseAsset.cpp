#include "BaseAsset.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Base Asset class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseAsset::WaitComplete() const {
	while (state_ != AssetState::Completed) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

bool BaseAsset::CheckAndBeginLoad() {
	std::unique_lock<std::mutex> lock(mutex_);
	
	if (state_ == AssetState::Unloaded) {
		state_ = AssetState::Loading;
		return true;
	}

	return false;
}

void BaseAsset::Complete() {
	state_ = AssetState::Completed;
}

void BaseAsset::Complete(const DirectXThreadContext* context) {
	context->ExecuteAllAllocators();
	Complete();
}
