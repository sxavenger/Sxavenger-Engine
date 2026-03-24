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
// FRenderPassDeferredBase class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief GBufferへの描画Pass
class FRenderPassDeferredBase
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

	//* opaque mesh render pass *//

	void BeginOpaqueMeshRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndOpaqueMeshRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void RenderStaticMesh(const DirectXQueueContext* context, const FRenderConfig& config);

	void RenderSkinnedMesh(const DirectXQueueContext* context, const FRenderConfig& config);

	//* motion vector pass *//

	void BeginMotionVectorPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndMotionVectorPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void PassMotionVector(const DirectXQueueContext* context, const FRenderConfig& config);

};

SXAVENGER_ENGINE_NAMESPACE_END


