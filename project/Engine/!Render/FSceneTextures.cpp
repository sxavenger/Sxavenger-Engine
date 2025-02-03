#include "FSceneTextures.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FSceneTextures::kGBufferLayoutCount> FSceneTextures::kGBufferFormats = {
	DXGI_FORMAT_R10G10B10A2_UNORM,  //!< Normal
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< Material_AO
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< Albedo
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< Position
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< Lighting
};

////////////////////////////////////////////////////////////////////////////////////////////
// Framework SceneTextures class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSceneTextures::Create(const Vector2ui& size) {
	for (uint8_t i = 0; i < kGBufferLayoutCount; ++i) {
		gBuffers_[i] = std::make_unique<FTexture>();
		gBuffers_[i]->Create(size, kGBufferFormats[i]);
	}

	depth_ = std::make_unique<FSceneDepth>();
	depth_->Create(size);

	parameterBuffer_ = std::make_unique<DxObject::DimensionBuffer<TexturesParameter>>();
	parameterBuffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameterBuffer_->At(0).size = size;

}

void FSceneTextures::BeginOpaqueBasePass(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Normal)]->TransitionBeginRenderTarget(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Material_AO)]->TransitionBeginRenderTarget(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Albedo)]->TransitionBeginRenderTarget(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Position)]->TransitionBeginRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionBeginRasterizer(context);

	static const uint8_t kGBufferCount = 4;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kGBufferCount> handles = {};
	handles[0] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Normal)]->GetCPUHandleRTV();
	handles[1] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Material_AO)]->GetCPUHandleRTV();
	handles[2] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Albedo)]->GetCPUHandleRTV();
	handles[3] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Position)]->GetCPUHandleRTV();

	context->GetCommandList()->OMSetRenderTargets(
		kGBufferCount, handles.data(), false, &depth_->GetRasterizerCPUHandleDSV()
	);

	gBuffers_[static_cast<uint8_t>(GBufferLayout::Normal)]->ClearRenderTarget(context);
	gBuffers_[static_cast<uint8_t>(GBufferLayout::Material_AO)]->ClearRenderTarget(context);
	gBuffers_[static_cast<uint8_t>(GBufferLayout::Albedo)]->ClearRenderTarget(context);
	gBuffers_[static_cast<uint8_t>(GBufferLayout::Position)]->ClearRenderTarget(context);

	depth_->ClearRasterizerDepth(context);
}

void FSceneTextures::EndOpaqueBasePass(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Normal)]->TransitionEndRenderTarget(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Material_AO)]->TransitionEndRenderTarget(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Albedo)]->TransitionEndRenderTarget(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Position)]->TransitionEndRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionEndRasterizer(context);
}

void FSceneTextures::BeginLightingPass(const DirectXThreadContext* context) const {

	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->TransitionBeginRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	static const uint8_t kGBufferCount = 1;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kGBufferCount> handles = {};
	handles[0] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->GetCPUHandleRTV();

	context->GetCommandList()->OMSetRenderTargets(
		kGBufferCount, handles.data(), false, nullptr
	);

	gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->ClearRenderTarget(context);
}

void FSceneTextures::EndLightingPass(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->TransitionEndRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);
}

void FSceneTextures::BeginTransparentBasePass(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->TransitionBeginRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionBeginRasterizer(context);

	static const uint8_t kGBufferCount = 1;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kGBufferCount> handles = {};
	handles[0] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->GetCPUHandleRTV();

	context->GetCommandList()->OMSetRenderTargets(
		kGBufferCount, handles.data(), false, &depth_->GetRasterizerCPUHandleDSV()
	);
}

void FSceneTextures::EndTransparentBasePass(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->TransitionEndRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionEndRasterizer(context);
}

void FSceneTextures::BeginPostProcessPass(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Normal)]->TransitionBeginState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Material_AO)]->TransitionBeginState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Albedo)]->TransitionBeginState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Position)]->TransitionBeginState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionBeginStateRasterizer(context, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void FSceneTextures::EndPostProcessPass(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Normal)]->TransitionEndState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Material_AO)]->TransitionEndState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Albedo)]->TransitionEndState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Position)]->TransitionEndState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionEndStateRasterizer(context, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void FSceneTextures::BeginCanvasPass(const DirectXThreadContext* context) const {

	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->TransitionBeginRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionBeginRasterizer(context);
	depth_->ClearRasterizerDepth(context);

	static const uint8_t kGBufferCount = 1;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kGBufferCount> handles = {};
	handles[0] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->GetCPUHandleRTV();

	context->GetCommandList()->OMSetRenderTargets(
		kGBufferCount, handles.data(), false, &depth_->GetRasterizerCPUHandleDSV()
	);

	depth_->ClearRasterizerDepth(context);

	//gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->ClearRenderTarget(context);

}

void FSceneTextures::EndCanvasPass(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->TransitionEndRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionEndRasterizer(context);

}

void FSceneTextures::BeginForward(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->TransitionBeginRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionBeginRasterizer(context);

	static const uint8_t kGBufferCount = 1;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kGBufferCount> handles = {};
	handles[0] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->GetCPUHandleRTV();

	context->GetCommandList()->OMSetRenderTargets(
		kGBufferCount, handles.data(), false, &depth_->GetRasterizerCPUHandleDSV()
	);
}

void FSceneTextures::EndForward(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->TransitionEndRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionEndRasterizer(context);
}
