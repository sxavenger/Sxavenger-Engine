#include "FRenderPassDeferredIndirectLighting.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FGBuffer.h"
#include "../Buffer/FLightAccumulationBuffer.h"
#include "../Core/FRenderCore.h"

//* engine
#include <Engine/System/Utility/RuntimeLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredIndirectLighting class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassDeferredIndirectLighting::Render(const DirectXQueueContext* context, const FRenderConfig& config) {

	if (config.globalIllumination == FRenderConfig::GlobalIllumination::None) {
		return; //!< 間接光が無効
	}

	if (config.HasIssue(FRenderConfig::IssueFlag::Warning_Geometry)) {
		return; //!< configが不適格
	}

	if (!config.buffer->HasBuffer<FGBuffer>()) {
		RuntimeLogger::LogError("[FRenderPass - Deferred Indirect Lighting]", "FGBuffer is required.");
		return; //!< Bufferが不適格
	}

	config.buffer->EnsureBuffer<FLightAccumulationBuffer>(); //!< Bufferの確保
	//!< RenderCoreは各Passで必要に応じて確保する.

	FBaseRenderPass::BeginRenderPass(context, "Deferred Indirect Lighting", config);

	{ //!< Indirect Lighting Pass

		switch (config.globalIllumination) {
			case FRenderConfig::GlobalIllumination::ReSTIR_PT:
				break;
		}
	}

	FBaseRenderPass::EndRenderPass(context);

}
