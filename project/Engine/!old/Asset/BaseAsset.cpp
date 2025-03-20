#include "BaseAsset.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

//* c++
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////
// Base Asset class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseAsset::WaitComplete() const {
	while (!IsComplete()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

bool BaseAsset::CheckAndBeginLoad() {
	if (state_ == State::Unloaded) {
		state_ = State::Loading;
		return true;
	}

	return false;
}
