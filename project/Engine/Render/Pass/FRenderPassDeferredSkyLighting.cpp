#include "FRenderPassDeferredSkyLighting.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FGBuffer.h"
#include "../Buffer/FLightAccumulationBuffer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredSkyLighting class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassDeferredSkyLighting::Render(const DirectXQueueContext* context, const FRenderConfig& config) {

	context;

	if (config.HasIssue(FRenderConfig::IssueFlag::Warning_Geometry)) {
		return; //!< configが不適格
	}

	if (!config.buffer->HasBuffer<FGBuffer>()) {
		RuntimeLogger::LogError("[FRenderPass - Deferred Direct Lighting]", "FGBuffer is required.");
		return; //!< Bufferが不適格
	}

}
