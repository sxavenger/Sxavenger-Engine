#include "FRenderPassForwardTransparent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"

//* engine
#include <Engine/Components/Component/MeshRenderer/MeshRendererComponent.h>
#include <Engine/Components/Component/MeshRenderer/SkinnedMeshRendererComponent.h>
//#include <Engine/Components/Component/Particle/ParticleComponent.h>
#include <Engine/Components/Component/ComponentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredBase class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassForwardTransparent::Render(const DirectXQueueContext* context, const Config& config) {

	// waning処理
	if (config.CheckStatus(FBaseRenderPass::Config::Status::Geometry_Warning)) {
		return;
	}

	context->BeginEvent(L"RenderPass - ForwardTransparent");

	BeginPassRenderTarget(context, config.buffer);

	PassStaticMesh(context, config);

	PassSkinnedMesh(context, config);

	EndPassRenderTarget(context, config.buffer);

	TransitionTransparentPass(context, config);

	context->EndEvent();

}

void FRenderPassForwardTransparent::BeginPassRenderTarget(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	std::array<D3D12_RESOURCE_BARRIER, FTransparentGBuffer::kLayoutCount> barriers = {
		buffer->GetGBuffer(FTransparentGBuffer::Layout::Accumulate)->TransitionBeginRenderTarget(),
		buffer->GetGBuffer(FTransparentGBuffer::Layout::Revealage)->TransitionBeginRenderTarget(),
	};

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	FDepthTexture* depth = buffer->GetDepth();
	depth->TransitionBeginRasterizer(context);

	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, FTransparentGBuffer::kLayoutCount> handles = {
		buffer->GetGBuffer(FTransparentGBuffer::Layout::Accumulate)->GetCPUHandleRTV(),
		buffer->GetGBuffer(FTransparentGBuffer::Layout::Revealage)->GetCPUHandleRTV(),
	};

	commandList->OMSetRenderTargets(
		static_cast<UINT>(handles.size()), handles.data(), false,
		&depth->GetRasterizerCPUHandleDSV()
	);

	buffer->GetGBuffer(FTransparentGBuffer::Layout::Accumulate)->ClearRenderTarget(context);
	buffer->GetGBuffer(FTransparentGBuffer::Layout::Revealage)->ClearRenderTarget(context);
}

void FRenderPassForwardTransparent::EndPassRenderTarget(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	FDepthTexture* depth = buffer->GetDepth();
	depth->TransitionEndRasterizer(context);

	std::array<D3D12_RESOURCE_BARRIER, FTransparentGBuffer::kLayoutCount> barriers = {
		buffer->GetGBuffer(FTransparentGBuffer::Layout::Accumulate)->TransitionEndRenderTarget(),
		buffer->GetGBuffer(FTransparentGBuffer::Layout::Revealage)->TransitionEndRenderTarget(),
	};

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

}

void FRenderPassForwardTransparent::PassStaticMesh(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetGeometry();
	core->SetPipeline(FRenderCoreGeometry::Type::Forward_MeshMS, context, config.buffer->GetSize());

	// common parameterの設定
	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gCamera",     config.camera->GetGPUVirtualAddress());
	parameter.SetAddress("gCullCamera", config.cullCamera->GetGPUVirtualAddress());

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

		parameter.SetAddress("gTransforms", transform->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  material->GetGPUVirtualAddress());
		//!< todo: materialをConstantBufferに変更する

		parameter.Set32bitConstants("Information", 1, &meshlet.meshletCount);
		parameter.SetAddress("gVertices",   mesh->GetInputVertex()->GetGPUVirtualAddress());
		parameter.SetAddress("gIndices",    meshlet.uniqueVertexIndices->GetGPUVirtualAddress());
		parameter.SetAddress("gMeshlets",   meshlet.meshlets->GetGPUVirtualAddress());
		parameter.SetAddress("gPrimitives", meshlet.primitiveIndices->GetGPUVirtualAddress());
		parameter.SetAddress("gBounds",     meshlet.meshletBounds->GetGPUVirtualAddress());
		 
		core->BindGraphicsBuffer(FRenderCoreGeometry::Type::Forward_MeshMS, context, parameter);
		meshlet.Dispatch(context, 1);

	});

}

void FRenderPassForwardTransparent::PassSkinnedMesh(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetGeometry();
	core->SetPipeline(FRenderCoreGeometry::Type::Forward_MeshVS, context, config.buffer->GetSize());

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

		if (material->GetMode() == AssetMaterial::Mode::Opaque) {
			return; //!< 透明なジオメトリは別のパスで描画
		}

		// メッシュの描画
		mesh->BindIABuffer(context);

		parameter.SetAddress("gTransforms", transform->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  material->GetGPUVirtualAddress());
		//!< todo: materialをConstantBufferに変更する
		 
		core->BindGraphicsBuffer(FRenderCoreGeometry::Type::Forward_MeshVS, context, parameter);

		mesh->DrawCall(context, 1);
		//!< todo: インスタンス描画対応

	});

}

void FRenderPassForwardTransparent::TransitionTransparentPass(const DirectXQueueContext* context, const Config& config) {

	config.buffer->TransitionBeginUnorderedMainScene(context);

	auto core = FRenderCore::GetInstance()->GetTransition();
	core->SetPipeline(FRenderCoreTransition::Transition::TransparentTransition, context);

	DxObject::BindBufferDesc parameter = {};
	// common parameter
	parameter.Set32bitConstants("Dimension", 2, &config.buffer->GetSize());

	// input
	parameter.SetHandle("gAccumulate", config.buffer->GetGBuffer(FTransparentGBuffer::Layout::Accumulate)->GetGPUHandleSRV());
	parameter.SetHandle("gRevealage",  config.buffer->GetGBuffer(FTransparentGBuffer::Layout::Revealage)->GetGPUHandleSRV());

	// output
	parameter.SetHandle("gOutput", config.buffer->GetGBuffer(FMainGBuffer::Layout::Scene)->GetGPUHandleUAV());

	core->BindComputeBuffer(FRenderCoreTransition::Transition::TransparentTransition, context, parameter);
	core->Dispatch(context, config.buffer->GetSize());

	config.buffer->TransitionEndUnorderedMainScene(context);

}

void FRenderPassForwardTransparent::PassParticles(const DirectXQueueContext* context, const Config& config) {
	context, config;

	/*sComponentStorage->ForEachActive<ParticleComponent>([&](ParticleComponent* component) {
		component->DrawParticle(context, config.camera);
	});*/

}
