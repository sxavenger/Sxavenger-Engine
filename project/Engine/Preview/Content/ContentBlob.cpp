#include "ContentBlob.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ContentBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ContentBlob::AsyncLoad(MAYBE_UNUSED const DirectXQueueContext* context) {
	BaseContent::CheckExist();

	// このcontentはparameter(profile)が設定されているのが前提
	Exception::Assert(param_.has_value(), "shader compile profile is not set.");

	DxObject::CompileProfile profile = std::any_cast<DxObject::CompileProfile>(param_);
	Load(BaseContent::GetFilepath(), profile);
}

void ContentBlob::AttachUuid() {
	BaseContent::CheckExist();
}

void ContentBlob::Load(const std::filesystem::path& filepath, const DxObject::CompileProfile& profile) {
	blob_.Create(filepath, profile);
	//!< entry pointはL"main"限定
}

const DxObject::ShaderBlob& ContentBlob::GetBlob() const {
	BaseContent::WaitComplete();
	return blob_;
}
