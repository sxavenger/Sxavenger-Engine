#include "FRenderPassForwardTransparent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FTransparentBuffer.h"
#include "../Buffer/FMainBuffer.h"
#include "../Core/FRenderCore.h"
#include "../Core/FRenderCoreGeometry.h"
#include "../Core/FRenderCoreTransition.h"

//* engine
#include <Engine/Components/Component/MeshRenderer/MeshRendererComponent.h>
#include <Engine/Components/Component/MeshRenderer/SkinnedMeshRendererComponent.h>
#include <Engine/Components/Component/ComponentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassForwardTransparent class
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassForwardTransparent::Render(const DirectXQueueContext* context, const FRenderConfig& config) {

	if (config.HasIssue(FRenderConfig::IssueFlag::Warning_Geometry)) {
		return; //!< configが不適格
	}

	if (!config.buffer->EnsureBuffer<FMainBuffer>()) {
		RuntimeLogger::LogError("[FRenderPass - Forward Transparent]", "MainBuffer is requires.");
		return; //!< MainBufferが確保できない場合は処理しない
	}

	config.buffer->EnsureBuffer<FTransparentBuffer>(); //!< Bufferの確保
	FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreGeometry>(); //!< RenderCoreの確保

	FBaseRenderPass::BeginRenderPass(context, "Forward Transparent", config);

	{ //!< Depth Pre-pass

		FBaseRenderPass::BeginEvent(context, "Depth Pre-pass");

		BeginDepthPrepass(context, config.buffer);

		RenderStaticMesh(context, config, Pass::DepthPrepass);

		RenderSkinnedMesh(context, config, Pass::DepthPrepass);

		EndDepthPrepass(context, config.buffer);

		FBaseRenderPass::EndEvent(context);
	}

	{ //!< Transparent Mesh Render Pass

		FBaseRenderPass::BeginEvent(context, "Transparent Mesh Render Pass");

		BeginTransparentMeshRenderPass(context, config.buffer);

		RenderStaticMesh(context, config, Pass::TransparentPass);

		RenderStaticMesh(context, config, Pass::TransparentPass);

		EndTransparentMeshRenderPass(context, config.buffer);

		FBaseRenderPass::EndEvent(context);
	}

	TransitionTransparentPass(context, config.buffer);

	FBaseRenderPass::EndRenderPass(context);

}

void FRenderPassForwardTransparent::BeginDepthPrepass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	FDepthStencilTexture* depthStencil = buffer->GetDepthStencil();

	{ //!< barrierの設定

		std::vector<D3D12_RESOURCE_BARRIER> barriers;

		//!< DepthStencilのbarrier設定
		depthStencil->SetTransitionDepthWrite(barriers);

		//!< barrierの発行
		context->GetDxCommand()->ResourceBarrier(barriers);
	}

	{ //!< Render Targetの設定

		commandList->OMSetRenderTargets(
			0, nullptr, false,
			&depthStencil->GetCPUHandleDSV()
		);
	}

}

void FRenderPassForwardTransparent::EndDepthPrepass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FDepthStencilTexture* depthStencil = buffer->GetDepthStencil();

	{ //!< barrierの設定

		std::vector<D3D12_RESOURCE_BARRIER> barriers;

		//!< DepthStencilのbarrier設定
		depthStencil->SetTransitionDefaultState(barriers);

		//!< barrierの発行
		context->GetDxCommand()->ResourceBarrier(barriers);
	}

}

void FRenderPassForwardTransparent::BeginTransparentMeshRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	FTransparentBuffer* transparentBuffer = buffer->GetBuffer<FTransparentBuffer>();
	FDepthStencilTexture* depthStencil    = buffer->GetDepthStencil();

	static const size_t	kBufferCount = FTransparentBuffer::kLayoutCount;
	std::array<FRenderTexture*, kBufferCount> buffers = {
		&transparentBuffer->GetBuffer(FTransparentBuffer::Layout::Accumulate),
		&transparentBuffer->GetBuffer(FTransparentBuffer::Layout::Revealage),
	};

	{ //!< barrierの設定

		std::vector<D3D12_RESOURCE_BARRIER> barriers;

		//!< RenderTargetのbarrier設定
		for (size_t i = 0; i < kBufferCount; ++i) {
			buffers[i]->SetTransitionRenderTarget(barriers);
		}

		//!< DepthStencilのbarrier設定
		depthStencil->SetTransitionDepthRead(barriers);

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

	//!< Transparent Buffer Render Targetのクリア
	for (size_t i = 0; i < kBufferCount; ++i) {
		buffers[i]->ClearRenderTarget(context);
	}
}

void FRenderPassForwardTransparent::EndTransparentMeshRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FTransparentBuffer* transparentBuffer = buffer->GetBuffer<FTransparentBuffer>();
	FDepthStencilTexture* depthStencil    = buffer->GetDepthStencil();

	static const size_t	kBufferCount = FTransparentBuffer::kLayoutCount;
	std::array<FRenderTexture*, kBufferCount> buffers = {
		&transparentBuffer->GetBuffer(FTransparentBuffer::Layout::Accumulate),
		&transparentBuffer->GetBuffer(FTransparentBuffer::Layout::Revealage),
	};

	{ //!< barrierの設定
		std::vector<D3D12_RESOURCE_BARRIER> barriers;

		//!< RenderTargetのbarrier設定
		for (size_t i = 0; i < kBufferCount; ++i) {
			buffers[i]->SetTransitionDefaultState(barriers);
		}

		//!< DepthStencilのbarrier設定
		depthStencil->SetTransitionDefaultState(barriers);

		//!< barrierの発行
		context->GetDxCommand()->ResourceBarrier(barriers);
	}

}

void FRenderPassForwardTransparent::TransitionTransparentPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FTransparentBuffer* transparent = buffer->GetBuffer<FTransparentBuffer>();
	FMainBuffer* main               = buffer->GetBuffer<FMainBuffer>();

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreTransition>(); //!< RenderCoreの確保
	core->SetPipeline(FRenderCoreTransition::Transition::TransparentTransition, context); //!< pipelineの設定

	main->GetBuffer(FMainBuffer::Layout::Scene).TransitionUnorderedAccess(context);

	{ //!< Transition Transparent Pass

		//!< parameterの設定
		DxObject::BindBufferDesc desc = {};

		//!< 共通parameterの設定
		desc.Set32bitConstants("Dimension", 2, &buffer->GetResolution());

		//!< Bufferの設定
		desc.SetHandle("gAccumulate", transparent->GetBuffer(FTransparentBuffer::Layout::Accumulate).GetGPUHandleSRV());
		desc.SetHandle("gRevealage",  transparent->GetBuffer(FTransparentBuffer::Layout::Revealage).GetGPUHandleSRV());
		desc.SetHandle("gOutput",     main->GetBuffer(FMainBuffer::Layout::Scene).GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreTransition::Transition::TransparentTransition, context, desc);
		core->Dispatch(context, buffer->GetResolution());
	}

	main->GetBuffer(FMainBuffer::Layout::Scene).TransitionDefaultState(context);
}

void FRenderPassForwardTransparent::RenderStaticMesh(const DirectXQueueContext* context, const FRenderConfig& config, Pass pass) {

	std::optional<FRenderCoreGeometry::Pipeline> pipeline = std::nullopt;

	//!< pipelineの選択
	switch (pass) {
		case Pass::DepthPrepass:
			pipeline = FRenderCoreGeometry::Pipeline::ForwardPrepass_MeshMS;
			break;

		case Pass::TransparentPass:
			pipeline = FRenderCoreGeometry::Pipeline::ForwardTransparent_MeshMS;
			break;

		default:
			StreamLogger::Exception("render static mesh invalid pass.");
	}

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreGeometry>();
	core->SetPipeline(pipeline.value(), context, config.buffer->GetResolution());

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

		const auto& meshlet = mesh->GetInputMesh().GetMeshlet();

		//!< 半透明ジオメトリ描画
		if (material->GetMode() != AssetMaterial::Mode::Translucent) {
			return;
		}

		
		desc.SetAddress("gTransform", transform->GetGPUVirtualAddress());
		desc.SetAddress("gMaterials", material->GetGPUVirtualAddress());
		//!< todo: materialをConstantBufferに変更する

		desc.Set32bitConstants("Information", 1, &meshlet.meshletCount);
		desc.SetAddress("gVertices",   mesh->GetInputVertex()->GetGPUVirtualAddress());
		desc.SetAddress("gIndices",    meshlet.uniqueVertexIndices->GetGPUVirtualAddress());
		desc.SetAddress("gMeshlets",   meshlet.meshlets->GetGPUVirtualAddress());
		desc.SetAddress("gPrimitives", meshlet.primitiveIndices->GetGPUVirtualAddress());
		desc.SetAddress("gBounds",     meshlet.meshletBounds->GetGPUVirtualAddress());
		 
		core->BindGraphicsBuffer(pipeline.value(), context, desc);
		meshlet.Dispatch(context, 1);
	});

}

void FRenderPassForwardTransparent::RenderSkinnedMesh(const DirectXQueueContext* context, const FRenderConfig& config, Pass pass) {

	std::optional<FRenderCoreGeometry::Pipeline> pipeline = std::nullopt;

	//!< pipelineの選択
	switch (pass) {
		case Pass::DepthPrepass:
			pipeline = FRenderCoreGeometry::Pipeline::ForwardPrepass_MeshVS;
			break;

		case Pass::TransparentPass:
			pipeline = FRenderCoreGeometry::Pipeline::ForwardTransparent_MeshVS;

		default:
			StreamLogger::Exception("render skinned mesh invalid pass.");
	}

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreGeometry>();
	core->SetPipeline(pipeline.value(), context, config.buffer->GetResolution());

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

		//!< 半透明ジオメトリ描画
		if (material->GetMode() != AssetMaterial::Mode::Translucent) {
			return;
		}

		// メッシュの描画
		component->BindInputAssembler(context);

		desc.Set32bitConstants("AddressBuffer", 2, &address);
		desc.SetAddress("gTransform", transform->GetGPUVirtualAddress());
		desc.SetAddress("gMaterials",  material->GetGPUVirtualAddress());
		//!< todo: materialをConstantBufferに変更する

		core->BindGraphicsBuffer(pipeline.value(), context, desc);
		component->DrawCall(context, 1);
	});

}
