#include "FRenderPassDeferredBase.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"

//* engine
#include <Engine/Component/Components/ComponentStorage.h>
#include <Engine/Component/Components/MeshRenderer/MeshRendererComponent.h>
#include <Engine/Component/Components/MeshRenderer/SkinnedMeshRendererComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredBase class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassDeferredBase::Render(const DirectXQueueContext* context, const Config& config) {

	// waning処理
	if (config.CheckStatus(FBaseRenderPass::Config::Status::Geometry_Warning)) {
		ClearPass(context, config.buffer);
		return;
	}

	context->BeginEvent(L"RenderPass - DeferredBase");

	{ //!< Render Target Pass
		BeginPassRenderTarget(context, config.buffer);

		PassStaticMesh(context, config);

		PassSkinnedMesh(context, config);

		EndPassRenderTarget(context, config.buffer);
	}
	
	{ //!< Velocity Pass
		BeginPassVelocity(context, config.buffer);

		PassVelocity(context, config);

		EndPassVelocity(context, config.buffer);
	}

	context->EndEvent();
}

void FRenderPassDeferredBase::BeginPassRenderTarget(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	std::array<FBaseTexture*, 4> buffers = {
		buffer->GetGBuffer(FDeferredGBuffer::Layout::Albedo),
		buffer->GetGBuffer(FDeferredGBuffer::Layout::Normal),
		buffer->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM),
		buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)
	};

	FDepthTexture* depth = buffer->GetDepth();

	std::array<D3D12_RESOURCE_BARRIER, 4> barriers = {};
	for (size_t i = 0; i < buffers.size(); ++i) {
		barriers[i] = buffers[i]->TransitionBeginRenderTarget();
	}

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	depth->TransitionBeginRasterizer(context);

	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 4> handles = {};
	for (size_t i = 0; i < buffers.size(); ++i) {
		handles[i] = buffers[i]->GetCPUHandleRTV();
	}

	commandList->OMSetRenderTargets(
		static_cast<UINT>(handles.size()), handles.data(), false,
		&depth->GetRasterizerCPUHandleDSV()
	);

	for (size_t i = 0; i < buffers.size(); ++i) {
		buffers[i]->ClearRenderTarget(context);
	}

	depth->ClearRasterizerDepth(context);

}

void FRenderPassDeferredBase::EndPassRenderTarget(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	std::array<FBaseTexture*, 4> buffers = {
		buffer->GetGBuffer(FDeferredGBuffer::Layout::Albedo),
		buffer->GetGBuffer(FDeferredGBuffer::Layout::Normal),
		buffer->GetGBuffer(FDeferredGBuffer::Layout::MaterialARM),
		buffer->GetGBuffer(FDeferredGBuffer::Layout::Position)
	};

	FDepthTexture* depth = buffer->GetDepth();

	depth->TransitionEndRasterizer(context);

	std::array<D3D12_RESOURCE_BARRIER, 4> barriers = {};
	for (size_t i = 0; i < buffers.size(); ++i) {
		barriers[i] = buffers[i]->TransitionEndRenderTarget();
	}

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

}

void FRenderPassDeferredBase::ClearPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	FDepthTexture* depth = buffer->GetDepth();

	//* RenderTargetへtransition *//

	std::array<D3D12_RESOURCE_BARRIER, FDeferredGBuffer::kLayoutCount> barriers = {};
	buffer->GetDeferredGBuffer().ForEach([&](size_t i, FBaseTexture* texture) {
		barriers[i] = texture->TransitionBeginRenderTarget();
	});

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	depth->TransitionBeginRasterizer(context);

	//* clear *//

	buffer->GetDeferredGBuffer().ForEach([&](FBaseTexture* texture) {
		texture->ClearRenderTarget(context);
	});

	depth->ClearRasterizerDepth(context);

	//* default stateへtransition *//

	depth->TransitionEndRasterizer(context);

	buffer->GetDeferredGBuffer().ForEach([&](size_t i, FBaseTexture* texture) {
		barriers[i] = texture->TransitionEndRenderTarget();
	});

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FRenderPassDeferredBase::PassStaticMesh(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetGeometry();
	core->SetPipeline(FRenderCoreGeometry::Type::Deferred_MeshMS, context, config.buffer->GetSize());

	// common parameterの設定
	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	sComponentStorage->ForEachActive<MeshRendererComponent>([&](MeshRendererComponent* component) {
		if (!component->IsEnable()) {
			return; //!< 不適格component.
		}

		auto transform = component->RequireTransform();

		auto mesh     = component->GetMesh();
		auto material = component->GetMaterial();

		const auto& meshlet = mesh->GetInputMesh().GetMeshlet();

		if (material->GetMode() != AssetMaterial::Mode::Opaque) {
			return; //!< 透明なジオメトリは別のパスで描画
		}

		parameter.SetAddress("gTransforms", transform->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  material->GetGPUVirtualAddress());
		//!< todo: materialをConstantBufferに変更する

		parameter.Set32bitConstants("Information", 1, &meshlet.meshletCount);
		parameter.SetAddress("gVertices",   mesh->GetInputVertex()->GetGPUVirtualAddress());
		parameter.SetAddress("gIndices",    meshlet.uniqueVertexIndices->GetGPUVirtualAddress());
		parameter.SetAddress("gMeshlets",   meshlet.meshlets->GetGPUVirtualAddress());
		parameter.SetAddress("gPrimitives", meshlet.primitiveIndices->GetGPUVirtualAddress());
		 
		core->BindGraphicsBuffer(FRenderCoreGeometry::Type::Deferred_MeshMS, context, parameter);
		meshlet.Dispatch(context, 1);

	});

}

void FRenderPassDeferredBase::PassSkinnedMesh(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetGeometry();
	core->SetPipeline(FRenderCoreGeometry::Type::Deferred_MeshVS, context, config.buffer->GetSize());

	// common parameterの設定
	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	sComponentStorage->ForEachActive<SkinnedMeshRendererComponent>([&](SkinnedMeshRendererComponent* component) {
		if (!component->IsEnable()) {
			return; //!< 不適格component.
		}

		auto transform = component->RequireTransform();

		auto material = component->GetMaterial();
		
		if (material->GetMode() != AssetMaterial::Mode::Opaque) {
			return; //!< 透明なジオメトリは別のパスで描画
		}

		// メッシュの描画
		component->BindIABuffer(context);

		parameter.SetAddress("gTransforms", transform->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  material->GetGPUVirtualAddress());
		//!< todo: materialをConstantBufferに変更する

		core->BindGraphicsBuffer(FRenderCoreGeometry::Type::Deferred_MeshVS, context, parameter);

		component->DrawCall(context, 1);
	

	});

}

void FRenderPassDeferredBase::BeginPassVelocity(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FBaseTexture* velocity = buffer->GetGBuffer(FDeferredGBuffer::Layout::Velocity);
	velocity->TransitionBeginUnordered(context);

}

void FRenderPassDeferredBase::EndPassVelocity(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FBaseTexture* velocity = buffer->GetGBuffer(FDeferredGBuffer::Layout::Velocity);
	velocity->TransitionEndUnordered(context);

}

void FRenderPassDeferredBase::PassVelocity(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetTransition();
	core->SetPipeline(FRenderCoreTransition::Transition::VelocityTransition, context);

	DxObject::BindBufferDesc parameter = {};
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());
	parameter.SetHandle("gPosition",       config.buffer->GetDeferredGBuffer().GetGBuffer(FDeferredGBuffer::Layout::Position)->GetGPUHandleSRV());
	parameter.SetAddress("gCurrentCamera", config.camera->GetGPUVirtualAddress());
	parameter.SetAddress("gPrevCamera",    config.camera->GetPrevGPUVirtualAddress());

	parameter.SetHandle("gVelocity", config.buffer->GetGBuffer(FDeferredGBuffer::Layout::Velocity)->GetGPUHandleUAV());

	core->BindComputeBuffer(FRenderCoreTransition::Transition::VelocityTransition, context, parameter);
	core->Dispatch(context, config.buffer->GetSize());

}
