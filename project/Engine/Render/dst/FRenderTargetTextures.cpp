#include "FRenderTargetTextures.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/Exporter/TextureExporter.h>

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FRenderTargetTextures::kGBufferLayoutCount> FRenderTargetTextures::kGBufferFormats_ = {
	DXGI_FORMAT_R10G10B10A2_UNORM,  //!< Normal
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< Material_AO
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< Albedo
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< Position
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< UI
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

	process_ = std::make_unique<FProcessTextures>();
	process_->Create(2, size, DXGI_FORMAT_R32G32B32A32_FLOAT);

	size_ = size;

	dimension_ = std::make_unique<DimensionBuffer<Vector2ui>>();
	dimension_->Create(SxavengerSystem::GetDxDevice(), 1);
	dimension_->At(0) = size;
}

void FRenderTargetTextures::ClearTextures(const DirectXQueueContext* context) const {
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

void FRenderTargetTextures::ClearTexturesPathtracing(const DirectXQueueContext* context) const {
	//* GBufferのクリア
	std::array<D3D12_RESOURCE_BARRIER, kGBufferLayoutCount> barriers = {};

	for (uint8_t i = 0; i < kGBufferLayoutCount; ++i) {
		barriers[i] = gBuffers_[i]->TransitionBeginRenderTarget();
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	for (uint8_t i = 0; i < kGBufferLayoutCount; ++i) {
		if (i == static_cast<uint8_t>(GBufferLayout::Main)) {
			continue;
		}

		gBuffers_[i]->ClearRenderTarget(context);
	}

	for (uint8_t i = 0; i < kGBufferLayoutCount; ++i) {
		barriers[i] = gBuffers_[i]->TransitionEndRenderTarget();
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	//* Depthのクリア
	depth_->TransitionBeginRasterizer(context);
	depth_->ClearRasterizerDepth(context);
	depth_->TransitionEndRasterizer(context);
}

void FRenderTargetTextures::BeginGeometryPass(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Normal)]->TransitionBeginRenderTarget(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::MaterialARM)]->TransitionBeginRenderTarget(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Albedo)]->TransitionBeginRenderTarget(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Position)]->TransitionBeginRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionBeginRasterizer(context);

	static const uint8_t kGBufferCount = 4;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kGBufferCount> handles = {};
	handles[0] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Normal)]->GetCPUHandleRTV();
	handles[1] = gBuffers_[static_cast<uint8_t>(GBufferLayout::MaterialARM)]->GetCPUHandleRTV();
	handles[2] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Albedo)]->GetCPUHandleRTV();
	handles[3] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Position)]->GetCPUHandleRTV();

	context->GetCommandList()->OMSetRenderTargets(
		kGBufferCount, handles.data(), false, &depth_->GetRasterizerCPUHandleDSV()
	);
}

void FRenderTargetTextures::EndGeometryPass(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Normal)]->TransitionEndRenderTarget(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::MaterialARM)]->TransitionEndRenderTarget(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Albedo)]->TransitionEndRenderTarget(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Position)]->TransitionEndRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionEndRasterizer(context);
}

void FRenderTargetTextures::SetupGeometryPass(const DirectXQueueContext* context) const {
	static const uint8_t kGBufferCount = 4;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kGBufferCount> handles = {};
	handles[0] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Normal)]->GetCPUHandleRTV();
	handles[1] = gBuffers_[static_cast<uint8_t>(GBufferLayout::MaterialARM)]->GetCPUHandleRTV();
	handles[2] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Albedo)]->GetCPUHandleRTV();
	handles[3] = gBuffers_[static_cast<uint8_t>(GBufferLayout::Position)]->GetCPUHandleRTV();

	context->GetCommandList()->OMSetRenderTargets(
		kGBufferCount, handles.data(), false, &depth_->GetRasterizerCPUHandleDSV()
	);
}

void FRenderTargetTextures::BeginLightingPass(const DirectXQueueContext* context) const {
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

void FRenderTargetTextures::EndLightingPass(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->TransitionEndRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);
}

void FRenderTargetTextures::BeginTransparentBasePass(const DirectXQueueContext* context) const {
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

void FRenderTargetTextures::EndTransparentBasePass(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->TransitionEndRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionEndRasterizer(context);
}

void FRenderTargetTextures::BeginCanvasPass(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::UI)]->TransitionBeginRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionBeginRasterizer(context);

	static const uint8_t kGBufferCount = 1;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kGBufferCount> handles = {};
	handles[0] = gBuffers_[static_cast<uint8_t>(GBufferLayout::UI)]->GetCPUHandleRTV();

	context->GetCommandList()->OMSetRenderTargets(
		kGBufferCount, handles.data(), false, &depth_->GetRasterizerCPUHandleDSV()
	);

	depth_->ClearRasterizerDepth(context);
}

void FRenderTargetTextures::EndCanvasPass(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::UI)]->TransitionEndRenderTarget(),
	};

	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionEndRasterizer(context);
}

void FRenderTargetTextures::BeginRaytracingPass(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::MaterialARM)]->TransitionBeginUnordered(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Normal)]->TransitionBeginUnordered(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::UI)]->TransitionBeginUnordered(), //!< 仮としてUIを使用
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->TransitionBeginUnordered(),
	};
	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionBeginRaytracing(context);
}

void FRenderTargetTextures::EndRaytracingPass(const DirectXQueueContext* context) const {
	D3D12_RESOURCE_BARRIER barriers[] = {
		gBuffers_[static_cast<uint8_t>(GBufferLayout::MaterialARM)]->TransitionEndUnordered(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Normal)]->TransitionEndUnordered(),
		gBuffers_[static_cast<uint8_t>(GBufferLayout::UI)]->TransitionEndUnordered(), //!< 仮としてUIを使用
		gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)]->TransitionEndUnordered(),
	};
	context->GetCommandList()->ResourceBarrier(_countof(barriers), barriers);

	depth_->TransitionEndRaytracing(context);
	depth_->CopyRaytracingToRasterizer(context);
}

void FRenderTargetTextures::BeginPostProcess(const DirectXQueueContext* context) {
	process_->BeginProcess(context, gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)].get());
}

void FRenderTargetTextures::EndPostProcess(const DirectXQueueContext* context) {
	process_->EndProcess(context, gBuffers_[static_cast<uint8_t>(GBufferLayout::Main)].get());
}

void FRenderTargetTextures::CaptureGBuffer(GBufferLayout layout, const DirectXQueueContext* context, const std::filesystem::path& filepath) const {

	auto texture = GetGBuffer(layout);

	texture->TransitionBeginState(context, D3D12_RESOURCE_STATE_COPY_SOURCE);

	TextureExporter::Export(
		context, TextureExporter::TextureDimension::Texture2D, texture->GetResource(), GetFormat(layout), filepath
	);

	texture->TransitionEndState(context, D3D12_RESOURCE_STATE_COPY_SOURCE);
}
