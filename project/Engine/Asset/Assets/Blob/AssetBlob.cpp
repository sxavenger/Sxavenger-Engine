#include "AssetBlob.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// Asset Blob class methods
//////////////////////////////////////////////////////////////////////////////////////////////

void AssetBlob::Load(_MAYBE_UNUSED const DirectXThreadContext* context) {
	if (BaseAsset::IsLoaded()) {
		return;
	}

	BaseAsset::SetState(BaseAsset::State::Loading);
	Assert(profile_.has_value(), "profile is not set.");

	DxObject::ShaderBlob::Create(filepath_, profile_.value());
	context->ExecuteAllAllocators();

	BaseAsset::SetState(BaseAsset::State::Complete);
}
