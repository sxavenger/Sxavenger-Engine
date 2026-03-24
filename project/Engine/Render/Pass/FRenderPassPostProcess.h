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
// FRenderPassPostProcess class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ポストプロセス処理Pass
class FRenderPassPostProcess
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

	//* post process pass *//

	void BeginPostProcessPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndPostProcessPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void PostProcessGlobal(const DirectXQueueContext* context, const FRenderConfig& config);

	void PostProcessVolume(const DirectXQueueContext* context, const FRenderConfig& config);

	void PostProcessLocal(const DirectXQueueContext* context, const FRenderConfig& config);

};



SXAVENGER_ENGINE_NAMESPACE_END
