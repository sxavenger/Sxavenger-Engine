#include "UContentBlob.h"

////////////////////////////////////////////////////////////////////////////////////////////
// UContentBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UContentBlob::AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) {
	UBaseContent::CheckExist();

	// このcontentはparameter(profile)が設定されているのが前提
	Exception::Assert(param_.has_value(), "shader compile profile is not set.");

	DxObject::CompileProfile profile = std::any_cast<DxObject::CompileProfile>(param_);
	Load(UBaseContent::GetFilepath(), profile);
}

void UContentBlob::AttachUuid() {
	UBaseContent::CheckExist();
}

void UContentBlob::Load(const std::filesystem::path& filepath, const DxObject::CompileProfile& profile) {
	blob_.Create(filepath, profile);
	//!< entry pointはL"main"限定
}

const DxObject::ShaderBlob& UContentBlob::GetBlob() const {
	UBaseContent::WaitComplete();
	return blob_;
}
