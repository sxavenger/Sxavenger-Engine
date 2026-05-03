#include "FRenderPassDeferredBase.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FGBuffer.h"
#include "../Buffer/FDepthStencilBuffer.h"
#include "../Core/FRenderCore.h"
#include "../Core/FRenderCoreGeometry.h"
#include "../Core/FRenderCoreTransition.h"
#include "../FPresenter.h"

//* engine
#include <Engine/Graphics/Graphics.h>
#include <Engine/Components/Component/MeshRenderer/MeshRendererComponent.h>
#include <Engine/Components/Component/MeshRenderer/SkinnedMeshRendererComponent.h>
#include <Engine/Components/Component/ComponentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredBase class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassDeferredBase::Render(const DirectXQueueContext* context, const FRenderConfig& config) {

	if (config.HasIssue(FRenderConfig::IssueFlag::Warning_Geometry)) {
		return; //!< configが不適格
	}

	config.buffer->EnsureBuffer<FGBuffer>(); //!< Bufferの確保
	FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreGeometry>();
	FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreTransition>(); //!< RenderCoreの確保.

	FBaseRenderPass::BeginRenderPass(context, "Deferred Base", config);

	{ //!< Opaque Mesh Render Pass

		FBaseRenderPass::BeginEvent(context, "Opaque Mesh Render Pass");

		BeginOpaqueMeshRenderPass(context, config.buffer);

		RenderStaticMesh(context, config);

		RenderSkinnedMesh(context, config);

		EndOpaqueMeshRenderPass(context, config.buffer);

		FBaseRenderPass::EndEvent(context);
	}

	{ //!< Motion Vector Pass

		FBaseRenderPass::BeginEvent(context, "Motion Vector Pass");

		BeginMotionVectorPass(context, config.buffer);

		PassMotionVector(context, config);

		EndMotionVectorPass(context, config.buffer);

		FBaseRenderPass::EndEvent(context);
	}

	if (config.option.Test(FRenderConfig::OptionFlag::LightingOnly)) {
		LightingOnly(context, config.buffer);
	}

	FBaseRenderPass::EndRenderPass(context);
}

void FRenderPassDeferredBase::BeginOpaqueMeshRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	FGBuffer* gbuffer                  = buffer->GetBuffer<FGBuffer>();
	FDepthStencilBuffer* depthStencil = buffer->GetBuffer<FDepthStencilBuffer>();

	static const size_t kBufferCount = 4;
	std::array<FRenderTexture*, kBufferCount> buffers = {
		&gbuffer->GetBuffer(FGBuffer::Layout::Albedo),
		&gbuffer->GetBuffer(FGBuffer::Layout::Normal),
		&gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM),
		&gbuffer->GetBuffer(FGBuffer::Layout::Address)
	};

	{ //!< barrierの設定

		std::vector<D3D12_RESOURCE_BARRIER> barriers;

		//!< GBufferのbarrier設定
		for (size_t i = 0; i < kBufferCount; ++i) {
			buffers[i]->SetTransitionRenderTarget(barriers);
		}

		//!< DepthStencilのbarrier設定
		depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).SetTransitionDepthWrite(barriers);

		//!< barrierの発行
		context->GetDxCommand()->ResourceBarrier(barriers);
	}

	{ //!< Render Targetの設定

		std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kBufferCount> handles = {};
		for (size_t i = 0; i < kBufferCount; ++i) {
			handles[i] = buffers[i]->GetCPUHandleRTV();
		}

		commandList->OMSetRenderTargets(
			static_cast<UINT>(handles.size()), handles.data(), false,
			&depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetCPUHandleDSV()
		);
	}

	//!< GBuffer Render Targetのクリア
	for (size_t i = 0; i < kBufferCount; ++i) {
		buffers[i]->ClearRenderTarget(context);
	}

}

void FRenderPassDeferredBase::EndOpaqueMeshRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FGBuffer* gbuffer                 = buffer->GetBuffer<FGBuffer>();
	FDepthStencilBuffer* depthStencil = buffer->GetBuffer<FDepthStencilBuffer>();

	static const size_t kBufferCount = 4;
	std::array<FRenderTexture*, kBufferCount> buffers = {
		&gbuffer->GetBuffer(FGBuffer::Layout::Albedo),
		&gbuffer->GetBuffer(FGBuffer::Layout::Normal),
		&gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM),
		&gbuffer->GetBuffer(FGBuffer::Layout::Address)
	};

	{ //!< barrierの設定

		std::vector<D3D12_RESOURCE_BARRIER> barriers;

		//!< GBufferのbarrier設定
		for (size_t i = 0; i < kBufferCount; ++i) {
			buffers[i]->SetTransitionDefaultState(barriers);
		}

		//!< DepthStencilのbarrier設定
		depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).SetTransitionDefaultState(barriers);

		//!< barrierの発行
		context->GetDxCommand()->ResourceBarrier(barriers);
	}

}

void FRenderPassDeferredBase::RenderStaticMesh(const DirectXQueueContext* context, const FRenderConfig& config) {

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreGeometry>();
	core->SetPipeline(FRenderCoreGeometry::Pipeline::Deferred_MeshMS, context, config.buffer->GetResolution());

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.SetAddress("gCamera",     config.camera->GetGPUVirtualAddress());
	desc.SetAddress("gCullCamera", config.cullCamera->GetGPUVirtualAddress());

	sComponentStorage->ForEachActive<MeshRendererComponent>([&](MeshRendererComponent* component) {

		if (!component->IsEnable()) {
			return; //!< 不適格component.
		}

		auto transform = component->RequireTransform();

		auto mesh     = component->GetMesh();
		auto material = component->GetMaterial();
		auto address  = component->GetBehaviourAddress();

		const auto& meshlet = mesh->GetInputMesh().GetMeshlet();

		//!< 不透明なジオメトリは別のパスで描画
		if (component->GetMode() != MeshRendererCommon::Mode::Opaque) {
			return;
		}

		desc.Set32bitConstants("AddressBuffer", 2, &address);
		desc.SetAddress("gTransform", transform->GetGPUVirtualAddress());
		desc.SetAddress("gMaterials", material->GetGPUVirtualAddress());
		//!< todo: materialをConstantBufferに変更する

		desc.Set32bitConstants("Information", 1, &meshlet.meshletCount);
		desc.SetAddress("gVertices",   mesh->GetInputVertex()->GetGPUVirtualAddress());
		desc.SetAddress("gIndices",    meshlet.uniqueVertexIndices->GetGPUVirtualAddress());
		desc.SetAddress("gMeshlets",   meshlet.meshlets->GetGPUVirtualAddress());
		desc.SetAddress("gPrimitives", meshlet.primitiveIndices->GetGPUVirtualAddress());
		desc.SetAddress("gBounds",     meshlet.meshletBounds->GetGPUVirtualAddress());
		 
		core->BindGraphicsBuffer(FRenderCoreGeometry::Pipeline::Deferred_MeshMS, context, desc);
		meshlet.Dispatch(context, 1);
	});

}

void FRenderPassDeferredBase::RenderSkinnedMesh(const DirectXQueueContext* context, const FRenderConfig& config) {

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreGeometry>();
	core->SetPipeline(FRenderCoreGeometry::Pipeline::Deferred_MeshVS, context, config.buffer->GetResolution());

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	sComponentStorage->ForEachActive<SkinnedMeshRendererComponent>([&](SkinnedMeshRendererComponent* component) {
		if (!component->IsEnable()) {
			return; //!< 不適格component.
		}

		auto transform = component->RequireTransform();
		auto material  = component->GetMaterial();
		auto address   = component->GetBehaviourAddress();

		//!< 不透明ジオメトリ描画
		if (component->GetMode() != MeshRendererCommon::Mode::Opaque) {
			return;
		}

		// メッシュの描画
		component->BindInputAssembler(context);

		desc.Set32bitConstants("AddressBuffer", 2, &address);
		desc.SetAddress("gTransform", transform->GetGPUVirtualAddress());
		desc.SetAddress("gMaterials",  material->GetGPUVirtualAddress());
		//!< todo: materialをConstantBufferに変更する

		core->BindGraphicsBuffer(FRenderCoreGeometry::Pipeline::Deferred_MeshVS, context, desc);
		component->DrawCall(context, 1);
	});

}

void FRenderPassDeferredBase::BeginMotionVectorPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FGBuffer* gbuffer = buffer->GetBuffer<FGBuffer>();

	//!< Motion Vector Bufferのbarrier設定
	gbuffer->GetBuffer(FGBuffer::Layout::MotionVector).TransitionUnorderedAccess(context);

}

void FRenderPassDeferredBase::EndMotionVectorPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FGBuffer* gbuffer = buffer->GetBuffer<FGBuffer>();

	//!< Motion Vector Bufferのbarrier設定
	gbuffer->GetBuffer(FGBuffer::Layout::MotionVector).TransitionDefaultState(context);

}

void FRenderPassDeferredBase::PassMotionVector(const DirectXQueueContext* context, const FRenderConfig& config) {

	FGBuffer* gbuffer                 = config.buffer->GetBuffer<FGBuffer>();
	FDepthStencilBuffer* depthStencil = config.buffer->GetBuffer<FDepthStencilBuffer>();

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreTransition>();
	core->SetPipeline(FRenderCoreTransition::Transition::MotionVectorTransition, context);

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetHandle("gMotionVector",   gbuffer->GetBuffer(FGBuffer::Layout::MotionVector).GetGPUHandleUAV());
	desc.SetHandle("gDepth",          depthStencil->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetGPUHandleSRV());
	desc.SetAddress("gCurrentCamera", config.camera->GetGPUVirtualAddress());
	desc.SetAddress("gPrevCamera",    config.camera->GetPrevGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreTransition::Transition::MotionVectorTransition, context, desc);
	core->Dispatch(context, config.buffer->GetResolution());

}

void FRenderPassDeferredBase::LightingOnly(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {
	//!< Lighting確認のため, Albedo Buffer を 白色で塗りつぶす.

	FGBuffer* gbuffer = buffer->GetBuffer<FGBuffer>();
	gbuffer->GetBuffer(FGBuffer::Layout::Albedo).TransitionUnorderedAccess(context);

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreTransition>(); //!< RenderCoreの確保.
	core->SetPipeline(FRenderCoreTransition::Transition::AlbedoWhiteTransition, context);

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, &buffer->GetResolution());
	desc.SetHandle("gAlbedo", gbuffer->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleUAV());

	core->BindComputeBuffer(FRenderCoreTransition::Transition::AlbedoWhiteTransition, context, desc);
	core->Dispatch(context, buffer->GetResolution());

	gbuffer->GetBuffer(FGBuffer::Layout::Albedo).TransitionDefaultState(context);
	
}

