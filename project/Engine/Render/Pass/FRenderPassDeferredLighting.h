#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderPass.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredLighting class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderPassDeferredLighting
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderPassDeferredLighting()           = default;
	~FRenderPassDeferredLighting() override = default;

	void Render(const DirectXQueueContext* context, const Config& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	void BeginPassDirectLighting(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndPassDirectLighting(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void BeginPassIndirectLighting(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndPassIndirectLighting(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void PassEmpty(const DirectXQueueContext* context, const Config& config);

	void PassDirectionalLight(const DirectXQueueContext* context, const Config& config);

	void PassPointLight(const DirectXQueueContext* context, const Config& config);

	void PassSkyLight(const DirectXQueueContext* context, const Config& config);

	void PassIndirectLight(const DirectXQueueContext* context, const Config& config);

	void LightingPassTransition(const DirectXQueueContext* context, const Config& config);

};
