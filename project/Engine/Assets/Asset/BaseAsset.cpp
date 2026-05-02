#include "BaseAsset.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/Utility/RuntimeLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseAsset class methods
////////////////////////////////////////////////////////////////////////////////////////////

const Uuid& BaseAsset::GetId() const {
	StreamLogger::AssertA(id_.has_value(), "asset does not have an id.");
	return id_.value();
}

std::string BaseAsset::SerializeId() const {
	return id_.has_value() ? id_->Serialize() : "null";
}

void BaseAsset::WaitComplete() const {
	while (status_ != Status::Complete) {
		RuntimeLogger::LogDebug("[BaseAsset]", "waiting for asset to complete loading... id: " + SerializeId());
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
