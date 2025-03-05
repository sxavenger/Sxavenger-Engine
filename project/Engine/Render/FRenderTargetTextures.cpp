#include "FRenderTargetTextures.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FRenderTargetTextures::kGBufferLayoutCount> FRenderTargetTextures::kGBufferFormats_ = {
	DXGI_FORMAT_R10G10B10A2_UNORM,  //!< Normal
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< Material_AO
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< Albedo
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< Position
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< Main
};

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderTargetTextures class
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderTargetTextures::Create(const Vector2ui& size) {

	for (uint8_t i = 0; i < kGBufferLayoutCount; ++i) {
		gBuffers_[i] = std::make_unique<FTexture>();
		gBuffers_[i]->Create(size, kGBufferFormats_[i]);
	}

	depth_ = std::make_unique<FDepthTexture>();
	depth_->Create(size);

	size_ = size;
}

void FRenderTargetTextures::ClearTextures(const DirectXThreadContext* context) const {
	//* GBufferのクリア
	std::array<D3D12_RESOURCE_BARRIER, kGBufferLayoutCount> barriers = {};

	for (uint8_t i = 0; i < kGBufferLayoutCount; ++i) {
		barriers[i] = gBuffers_[i]->TransitionBeginRenderTarget();
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	for (uint8_t i = 0; i < kGBufferLayoutCount; ++i) {
		gBuffers_[i]->ClearRenderTarget(context);
		barriers[i] = gBuffers_[i]->TransitionEndRenderTarget();
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	//* Depthのクリア
	depth_->TransitionBeginRasterizer(context);
	depth_->ClearRasterizerDepth(context);
	depth_->TransitionEndRasterizer(context);
}

void FRenderTargetTextures::BeginGeometryPass(const DirectXThreadContext* context) const {
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
}

void FRenderTargetTextures::EndGeometryPass(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Normal)]->TransitionEndRenderTarget(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Material_AO)]->TransitionEndRenderTarget(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Albedo)]->TransitionEndRenderTarget(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Position)]->TransitionEndRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionEndRasterizer(context);
}

void FRenderTargetTextures::SetupGeometryPass(const DirectXThreadContext* context) const {
	static const uint8_t kGBufferCount = 4;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kGBufferCount> handles = {};
	handles[0] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Normal)]->GetCPUHandleRTV();
	handles[1] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Material_AO)]->GetCPUHandleRTV();
	handles[2] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Albedo)]->GetCPUHandleRTV();
	handles[3] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Position)]->GetCPUHandleRTV();

	context->GetCommandList()->OMSetRenderTargets(
		kGBufferCount, handles.data(), false, &depth_->GetRasterizerCPUHandleDSV()
	);
}

void FRenderTargetTextures::BeginLightingPass(const DirectXThreadContext* context) const {
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

void FRenderTargetTextures::EndLightingPass(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->TransitionEndRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);
}

void FRenderTargetTextures::BeginTransparentBasePass(const DirectXThreadContext* context) const {
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

void FRenderTargetTextures::EndTransparentBasePass(const DirectXThreadContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->TransitionEndRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionEndRasterizer(context);
}
