#include "FBaseRenderPass.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////////////////
// Config structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void FBaseRenderPass::Config::AttachStatus() {

	status = Status::Success;

	if (buffer == nullptr) {
		status |= Status::Error_GBuffer;
		Logger::WarningRuntime("warning | [FBaseRenderPass]::Config::CheckStatus", "buffer is not set.");
	}

	if (scene == nullptr) {
		status |= Status::Warning_Scene;
		Logger::WarningRuntime("warning | [FBaseRenderPass]::Config::CheckStatus", "scene is not set.");
	}

	if (camera == nullptr) {
		status |= Status::Warning_Camera;
		Logger::WarningRuntime("warning | [FBaseRenderPass]::Config::CheckStatus", "camera is not set.");
	}

}

bool FBaseRenderPass::Config::CheckStatus(Status _status) const {
	return status.Any(_status);
}
