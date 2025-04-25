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
	Assert(param_.has_value(), "profile is not set.");
	DxObject::ShaderBlob::Create(filepath_, std::any_cast<DxObject::CompileProfile>(param_));
}
