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
// FRenderPassForwardTransparent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 透過物体の描画Pass
class FRenderPassForwardTransparent
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Pass enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Pass : uint8_t {
		DepthPrepass,
		TransparentPass,
	};

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* depth pre-pass *//

	void BeginDepthPrepass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndDepthPrepass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//* transparent mesh render pass *//

	void BeginTransparentMeshRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndTransparentMeshRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//* transition transparent pass *//

	void TransitionTransparentPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//* mesh render helper methods *//

	void RenderStaticMesh(const DirectXQueueContext* context, const FRenderConfig& config, Pass pass);

	void RenderSkinnedMesh(const DirectXQueueContext* context, const FRenderConfig& config, Pass pass);

};

SXAVENGER_ENGINE_NAMESPACE_END
