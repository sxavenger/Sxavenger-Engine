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
#include "Common/FPriorityTexture.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderTargetBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderTargetBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// DeferredBufferIndex structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct DeferredBufferIndex {
		uint32_t albedo;
		uint32_t normal;
		uint32_t materialARM;
		uint32_t position;
		uint32_t velocity;
		uint32_t depth;
	};

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

	void BeginUnorderedLightingIndirect(const DirectXQueueContext* context);
	void EndUnorderedLightingIndirect(const DirectXQueueContext* context);

	void BeginUnorderedMainScene(const DirectXQueueContext* context);
	void EndUnorderedMainScene(const DirectXQueueContext* context);

	void BeginRenderTargetMainTransparent(const DirectXQueueContext* context);
	void EndRenderTargetMainTransparent(const DirectXQueueContext* context);

	void BeginRenderTargetMainUI(const DirectXQueueContext* context);
	void EndRenderTargetMainUI(const DirectXQueueContext* context);

	void BeginPostProcess(const DirectXQueueContext* context);
	void EndPostProcess(const DirectXQueueContext* context);

	void BeginProcessDenoiser(const DirectXQueueContext* context);
	void EndProcessDenoiser(const DirectXQueueContext* context);

	//* GBuffer option *//

	FBaseTexture* GetGBuffer(FDeferredGBuffer::Layout layout);
	FBaseTexture* GetGBuffer(FLightingGBuffer::Layout layout);
	FBaseTexture* GetGBuffer(FMainGBuffer::Layout layout);

	FDeferredGBuffer& GetDeferredGBuffer() { return deferred_; }

	FLightingGBuffer& GetLightingGBuffer() { return lighting_; }

	FMainGBuffer& GetMainGBuffer() { return main_; }

	const D3D12_GPU_VIRTUAL_ADDRESS& GetIndexBufferAddress() const;

	//* depth stencil option *//

	FDepthTexture* GetDepth() { return depth_.get(); }

	FPriorityTexture* GetPriority() { return priority_.get(); }

	//* process texture option *//

	FProcessTextureCollection* GetProcessTextures() { return process_.get(); }

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
	std::unique_ptr<FPriorityTexture> priority_;

	//* process texture *//

	std::unique_ptr<FProcessTextureCollection> process_;

	//* parameter *//

	Vector2ui size_;
	//!< "Dimension" 32bitConstants で使用

	std::unique_ptr<DxObject::DimensionBuffer<DeferredBufferIndex>> index_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void AttachIndex();

};
