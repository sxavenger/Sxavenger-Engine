#include "ContentBlob.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ContentBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ContentBlob::Load(MAYBE_UNUSED const DirectXQueueContext* context) {

	DxObject::CompileProfile profile = GetProfile();

	blob_.Create(BaseContent::GetFilepath(), profile, L"main");
	//!< entry pointは"main"限定

	BaseContent::SetComplete(); //!< 読み込み完了
}

const DxObject::ShaderBlob& ContentBlob::GetBlob() const {
	BaseContent::WaitComplete();
	return blob_;
}

DxObject::CompileProfile ContentBlob::GetProfile() const {
	StreamLogger::AssertA(parameter_.has_value(), "compile profile is not specified. filepath: " + BaseContent::GetFilepath().generic_string());
	return std::any_cast<DxObject::CompileProfile>(parameter_);
}
