#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FRenderConfig.h"
#include "FBaseRenderPass.h"
#include "../Buffer/FRenderTargetBuffer.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredGlobalIllumination class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 間接光の描画Pass
class FRenderPassDeferredGlobalIllumination // [FRenderPassDeferredGlobalIllumination]に変更するかも...?
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	void Init() override;

	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* global illumination pass *//

	void BeginGlobalIlluminationPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndGlobalIlluminationPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//* Lux pass *//

	void BeginPassLux(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndPassLux(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void PassLuxProbeTrace(const DirectXQueueContext* context, const FRenderConfig& config);

	void PassLuxHistory(const DirectXQueueContext* context, const FRenderConfig& config);

	void PassLuxSolve(const DirectXQueueContext* context, const FRenderConfig& config);

	//* ReSTIR pass *//

	void BeginPassReSTIR(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndPassReSTIR(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void PassReSTIRResetReservoir(const DirectXQueueContext* context, const FRenderConfig& config);

	void PassReSTIRInitialReservoir(const DirectXQueueContext* context, const FRenderConfig& config);

	void PassReSTIRTemporalReuse(const DirectXQueueContext* context, const FRenderConfig& config);

	void PassReSTIRSolve(const DirectXQueueContext* context, const FRenderConfig& config);

};

SXAVENGER_ENGINE_NAMESPACE_END
