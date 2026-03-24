#include "FRenderPassDeferredBase.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FGBuffer.h"
#include "../Core/FRenderCore.h"
#include "../Core/FRenderCoreGeometry.h"
#include "../Core/FRenderCoreTransition.h"

//* engine
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
	FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreGeometry>(); //!< RenderCoreの確保

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

	FBaseRenderPass::EndRenderPass(context);
}

void FRenderPassDeferredBase::BeginOpaqueMeshRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	FGBuffer* gbuffer                  = buffer->GetBuffer<FGBuffer>();
	FDepthStencilTexture* depthStencil = buffer->GetDepthStencil();

	static const size_t kBufferCount = 5;
	std::array<FRenderTexture*, kBufferCount> buffers = {
		&gbuffer->GetBuffer(FGBuffer::Layout::Albedo),
		&gbuffer->GetBuffer(FGBuffer::Layout::Normal),
		&gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM),
		&gbuffer->GetBuffer(FGBuffer::Layout::Position),
		&gbuffer->GetBuffer(FGBuffer::Layout::Address)
	};

	{ //!< barrierの設定

		std::vector<D3D12_RESOURCE_BARRIER> barriers;

		//!< GBufferのbarrier設定
		for (size_t i = 0; i < kBufferCount; ++i) {
			buffers[i]->SetTransitionRenderTarget(barriers);
		}

		//!< DepthStencilのbarrier設定
		depthStencil->SetTransitionDepthWrite(barriers);

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
			&depthStencil->GetCPUHandleDSV()
		);
	}

	//!< GBuffer Render Targetのクリア
	for (size_t i = 0; i < kBufferCount; ++i) {
		buffers[i]->ClearRenderTarget(context);
	}

}

void FRenderPassDeferredBase::EndOpaqueMeshRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FGBuffer* gbuffer                  = buffer->GetBuffer<FGBuffer>();
	FDepthStencilTexture* depthStencil = buffer->GetDepthStencil();

	static const size_t kBufferCount = 5;
	std::array<FRenderTexture*, kBufferCount> buffers = {
		&gbuffer->GetBuffer(FGBuffer::Layout::Albedo),
		&gbuffer->GetBuffer(FGBuffer::Layout::Normal),
		&gbuffer->GetBuffer(FGBuffer::Layout::MaterialARM),
		&gbuffer->GetBuffer(FGBuffer::Layout::Position),
		&gbuffer->GetBuffer(FGBuffer::Layout::Address)
	};

	{ //!< barrierの設定

		std::vector<D3D12_RESOURCE_BARRIER> barriers;

		//!< GBufferのbarrier設定
		for (size_t i = 0; i < kBufferCount; ++i) {
			buffers[i]->SetTransitionDefaultState(barriers);
		}

		//!< DepthStencilのbarrier設定
		depthStencil->SetTransitionDefaultState(barriers);

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
		if (material->GetMode() != AssetMaterial::Mode::Opaque) {
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
		if (material->GetMode() != AssetMaterial::Mode::Opaque) {
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

	FGBuffer* gbuffer = config.buffer->GetBuffer<FGBuffer>();

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreTransition>();
	core->SetPipeline(FRenderCoreTransition::Transition::MotionVectorTransition, context);

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, &config.buffer->GetResolution());
	desc.SetHandle("gMotionVector",   gbuffer->GetBuffer(FGBuffer::Layout::MotionVector).GetGPUHandleUAV());
	desc.SetHandle("gPosition",       gbuffer->GetBuffer(FGBuffer::Layout::Position).GetGPUHandleSRV());
	desc.SetAddress("gCurrentCamera", config.camera->GetGPUVirtualAddress());
	desc.SetAddress("gPrevCamera",    config.camera->GetPrevGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreTransition::Transition::MotionVectorTransition, context, desc);
	core->Dispatch(context, config.buffer->GetResolution());

}

