#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "GBuffer/FDeferredGBuffer.h"
#include "GBuffer/FLightingGBuffer.h"
#include "GBuffer/FMainGBuffer.h"
#include "Common/FDepthTexture.h"
#include "Common/FProcessTexture.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderTargetBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderTargetBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(const Vector2ui& size);

	//* transition option *//

	void BeginRenderTargetDeferred(const DirectXQueueContext* context);
	void EndRenderTargetDeferred(const DirectXQueueContext* context);

	void BeginRenderTargetLightingDirect(const DirectXQueueContext* context);
	void EndRenderTargetLightingDirect(const DirectXQueueContext* context);

	void BeginUnorderedMainScene(const DirectXQueueContext* context);
	void EndUnorderedMainScene(const DirectXQueueContext* context);

	void BeginRenderTargetMainTransparent(const DirectXQueueContext* context);
	void EndRenderTargetMainTransparent(const DirectXQueueContext* context);

	void BeginRenderTargetMainUI(const DirectXQueueContext* context);
	void EndRenderTargetMainUI(const DirectXQueueContext* context);

	//* GBuffer option *//

	FBaseTexture* GetGBuffer(FDeferredGBuffer::Layout layout);
	FBaseTexture* GetGBuffer(FLightingGBuffer::Layout layout);
	FBaseTexture* GetGBuffer(FMainGBuffer::Layout layout);

	//* depth stencil option *//

	FDepthTexture* GetDepth() { return depth_.get(); }

	//* process texture option *//

	FProcessTextures* GetProcessTextures() { return process_.get(); }

	//* parameter option *//

	const Vector2ui& GetSize() const { return size_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* GBuffer parameter *//

	FDeferredGBuffer deferred_;
	FLightingGBuffer lighting_;
	FMainGBuffer     main_;

	//* depth stencil parameter *//

	std::unique_ptr<FDepthTexture> depth_;

	//* process texture *//

	std::unique_ptr<FProcessTextures> process_;

	//* parameter *//

	Vector2ui size_;
	//!< "Dimension" 32bitConstants で使用

};
