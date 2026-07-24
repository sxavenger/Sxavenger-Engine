#include "FRenderConfig.h"
SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderConfig structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Sxl::Flag<FRenderConfig::IssueFlag> FRenderConfig::CheckIssue(const FRenderConfig& config) {

	Sxl::Flag<FRenderConfig::IssueFlag> flags = IssueFlag::Success;

	if (config.buffer == nullptr) {
		flags |= IssueFlag::Error_Buffer;
	}

	if (config.scene == nullptr) {
		flags |= IssueFlag::Warning_Scene;
	}

	if (config.camera == nullptr || config.cullCamera == nullptr) {
		flags |= IssueFlag::Warning_Camera;
	}

	return flags;
}
