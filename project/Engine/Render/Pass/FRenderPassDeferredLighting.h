#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderPass.h"

//* engine
#include <Engine/Foundation.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredLighting class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderPassDeferredLighting
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Render(const DirectXQueueContext* context, const Config& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	void BeginPassDirectLighting(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndPassDirectLighting(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void BeginPassIndirectLighting(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndPassIndirectLighting(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void ClearPassDirect(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void ClearPassIndirect(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void PassEmpty(const DirectXQueueContext* context, const Config& config);
	void PassDirectionalLight(const DirectXQueueContext* context, const Config& config);
	void PassPointLight(const DirectXQueueContext* context, const Config& config);
	void PassSpotLight(const DirectXQueueContext* context, const Config& config);
	void PassSkyLight(const DirectXQueueContext* context, const Config& config);
	void PassSkyAtmosphere(const DirectXQueueContext* context, const Config& config);

	void PassIndirectReservoirReset(const DirectXQueueContext* context, const Config& config);
	void PassIndirectReservoirInitialize(const DirectXQueueContext* context, const Config& config);
	void PassIndirectReservoirTemporal(const DirectXQueueContext* context, const Config& config);
	void PassIndirectReservoirSpatial(const DirectXQueueContext* context, const Config& config);
	void PassIndirectReservoirTexture(const DirectXQueueContext* context, const Config& config);

	void PassProbeReservoir(const DirectXQueueContext* context, const Config& config);
	void PassProbeUpdate(const DirectXQueueContext* context, const Config& config);
	void PassProbeEvaluation(const DirectXQueueContext* context, const Config& config);

	void TransitionLightingPass(const DirectXQueueContext* context, const Config& config);



};

SXAVENGER_ENGINE_NAMESPACE_END

