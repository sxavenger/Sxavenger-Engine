#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "Common/FDepthTexture.h"
#include "Common/FProcessTexture.h"
#include "Common/FPriorityTexture.h"
#include "GBuffer/FDeferredGBuffer.h"
#include "GBuffer/FLightingGBuffer.h"
#include "GBuffer/FTransparentGBuffer.h"
#include "GBuffer/FProbeGBuffer.h"
#include "GBuffer/FMainGBuffer.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxConstantBuffer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

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

	void TransitionBeginRenderTargetMainScene(const DirectXQueueContext* context);
	void TransitionEndRenderTargetMainScene(const DirectXQueueContext* context);

	void TransitionBeginUnorderedMainScene(const DirectXQueueContext* context);
	void TransitionEndUnorderedMainScene(const DirectXQueueContext* context);

	void TransitionBeginRenderTargetMainCanvas(const DirectXQueueContext* context);
	void TransitionEndRenderTargetMainCanvas(const DirectXQueueContext* context);

	void ClearRenderTargetMain(const DirectXQueueContext* context);

	void BeginPostProcess(const DirectXQueueContext* context);
	void EndPostProcess(const DirectXQueueContext* context);

	//* GBuffer option *//

	FBaseTexture* GetGBuffer(FDeferredGBuffer::Layout layout);
	FBaseTexture* GetGBuffer(FLightingGBuffer::Layout layout);
	FBaseTexture* GetGBuffer(FTransparentGBuffer::Layout layout);
	FBaseTexture* GetGBuffer(FMainGBuffer::Layout layout);

	FDeferredGBuffer& GetDeferredGBuffer() { return deferred_; }

	FLightingGBuffer& GetLightingGBuffer() { return lighting_; }

	FProbeGBuffer& GetProbeGBuffer() { return probe_; }

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

	FDeferredGBuffer    deferred_;
	FLightingGBuffer    lighting_;
	FTransparentGBuffer transparent_;
	FMainGBuffer        main_;

	FProbeGBuffer probe_;

	//* depth stencil parameter *//

	std::unique_ptr<FDepthTexture> depth_;
	std::unique_ptr<FPriorityTexture> priority_;

	//* process texture *//

	std::unique_ptr<FProcessTextureCollection> process_;

	//* parameter *//

	Vector2ui size_;
	//!< "Dimension" 32bitConstants で使用

	std::unique_ptr<DxObject::ConstantBuffer<DeferredBufferIndex>> index_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void AttachIndex();

};

SXAVENGER_ENGINE_NAMESPACE_END
