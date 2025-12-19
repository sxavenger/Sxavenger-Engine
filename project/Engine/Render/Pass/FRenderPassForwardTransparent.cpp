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

	context->BeginEvent(L"RenderPass - ForwardTransparent");

	// waning処理
	if (config.CheckStatus(FBaseRenderPass::Config::Status::Geometry_Warning)) {
		ClearPass(context, config.buffer);
		return;
	}

	BeginPassRenderTarget(context, config.buffer);

	PassStaticMesh(context, config);

	PassSkinnedMesh(context, config);

	PassParticles(context, config);

	EndPassRenderTarget(context, config.buffer);

	context->EndEvent();

}

void FRenderPassForwardTransparent::BeginPassRenderTarget(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	std::array<FBaseTexture*, 1> buffers = {
		buffer->GetGBuffer(FMainGBuffer::Layout::Scene),
	};

	FDepthTexture* depth = buffer->GetDepth();

	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	for (size_t i = 0; i < buffers.size(); ++i) {
		barriers[i] = buffers[i]->TransitionBeginRenderTarget();
	}

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	depth->TransitionBeginRasterizer(context);

	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 1> handles = {};
	for (size_t i = 0; i < buffers.size(); ++i) {
		handles[i] = buffers[i]->GetCPUHandleRTV();
	}

	commandList->OMSetRenderTargets(
		static_cast<UINT>(handles.size()), handles.data(), false,
		&depth->GetRasterizerCPUHandleDSV()
	);

}

void FRenderPassForwardTransparent::EndPassRenderTarget(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	std::array<FBaseTexture*, 1> buffers = {
		buffer->GetGBuffer(FMainGBuffer::Layout::Scene),
	};

	FDepthTexture* depth = buffer->GetDepth();

	depth->TransitionEndRasterizer(context);

	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	for (size_t i = 0; i < buffers.size(); ++i) {
		barriers[i] = buffers[i]->TransitionEndRenderTarget();
	}

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

}

void FRenderPassForwardTransparent::ClearPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	auto commandList = context->GetCommandList();

	std::array<FBaseTexture*, 1> buffers = {
		buffer->GetGBuffer(FMainGBuffer::Layout::Scene),
	};

	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	for (size_t i = 0; i < buffers.size(); ++i) {
		barriers[i] = buffers[i]->TransitionBeginRenderTarget();
	}

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	for (size_t i = 0; i < buffers.size(); ++i) {
		buffers[i]->ClearRenderTarget(context);
	}
	
	for (size_t i = 0; i < buffers.size(); ++i) {
		barriers[i] = buffers[i]->TransitionEndRenderTarget();
	}

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

}

void FRenderPassForwardTransparent::PassStaticMesh(const DirectXQueueContext* context, const Config& config) {

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
			return; //!< 不透明なジオメトリは別のパスで描画
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

void FRenderPassForwardTransparent::PassParticles(const DirectXQueueContext* context, const Config& config) {
	context, config;

	/*sComponentStorage->ForEachActive<ParticleComponent>([&](ParticleComponent* component) {
		component->DrawParticle(context, config.camera);
	});*/

}
