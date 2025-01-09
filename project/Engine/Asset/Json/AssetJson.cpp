#include "AssetJson.h"

////////////////////////////////////////////////////////////////////////////////////////////
// AssetJson class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetJson::Load(_MAYBE_UNUSED const DirectXThreadContext* context) {
	if (!BaseAsset::CheckAndBeginLoad()) {
		return;
	}

	isSuccess_ = JsonAdapter::TryLoadFromJson(filepath_, json_);

	BaseAsset::Complete();
}

void AssetJson::Term() {
}
