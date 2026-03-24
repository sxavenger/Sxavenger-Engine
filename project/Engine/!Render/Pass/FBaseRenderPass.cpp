#include "FBaseRenderPass.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/RuntimeLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Config structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void FBaseRenderPass::Config::AttachStatus() {

	status = Status::Success;

	if (buffer == nullptr) {
		status |= Status::Error_GBuffer;
		RuntimeLogger::LogWarning("[FBaseRenderPass]", "buffer is not set.");
	}

	if (scene == nullptr) {
		status |= Status::Warning_Scene;
		RuntimeLogger::LogWarning("[FBaseRenderPass]", "scene is not set.");
	}

	if (camera == nullptr || cullCamera == nullptr) {
		status |= Status::Warning_Camera;
		RuntimeLogger::LogWarning("[FBaseRenderPass]", "camera is not set.");
	}

}

bool FBaseRenderPass::Config::CheckStatus(Status _status) const {
	return status.Any(_status);
}
