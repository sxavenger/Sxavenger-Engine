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
// FRenderPassDeferredDirectLighting class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 直接光の描画Pass
class FRenderPassDeferredDirectLighting
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* direct lighting pass *//

	void BeginDirectLightingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndDirectLightingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void PassUnlit(const DirectXQueueContext* context, const FRenderConfig& config);

	void PassDirectionalLight(const DirectXQueueContext* context, const FRenderConfig& config);

	void PassPointLight(const DirectXQueueContext* context, const FRenderConfig& config);

	void PassSpotLight(const DirectXQueueContext* context, const FRenderConfig& config);

	void PassRectLight(const DirectXQueueContext* context, const FRenderConfig& config);

	void PassSkyLight(const DirectXQueueContext* context, const FRenderConfig& config);

	void PassSkyAtmosphere(const DirectXQueueContext* context, const FRenderConfig& config);

};

SXAVENGER_ENGINE_NAMESPACE_END
