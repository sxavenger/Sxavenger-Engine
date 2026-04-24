#include "BaseAsset.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseAsset class methods
////////////////////////////////////////////////////////////////////////////////////////////

const Uuid& Asset::BaseAsset::GetId() const {
	StreamLogger::AssertA(id_.has_value(), "asset does not have an id.");
	return id_.value();
}

std::string Asset::BaseAsset::SerializeId() const {
	return id_.has_value() ? id_->Serialize() : "null";
}

void Asset::BaseAsset::WaitComplete() const {
	while (status_ != Status::Complete) {
		std::this_thread::sleep_for(std::chrono::microseconds(2));
	}
}
