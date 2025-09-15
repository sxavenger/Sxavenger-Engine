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

	config.buffer->BeginRenderTargetDeferred(context);

	PassStaticMesh(context, config);

	PassSkinnedMesh(context, config);

	config.buffer->EndRenderTargetDeferred(context);

}

void FRenderPassDeferredBase::PassStaticMesh(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetGeometry();
	core->SetPipeline(FRenderCoreGeometry::Type::Deferred_MeshVS, context, config.buffer->GetSize());

	// common parameterの設定
	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	sComponentStorage->ForEachActive<MeshRendererComponent>([&](MeshRendererComponent* component) {
		if (!component->IsEnabled()) {
			return; //!< 不適格component.
		}

		auto transform = component->GetTransform();

		auto mesh     = component->GetMesh();
		auto material = component->GetMaterial();

		if (material->GetMode() != UAssetMaterial::Mode::Opaque) {
			return; //!< 透明なジオメトリは別のパスで描画
		}

		// メッシュの描画
		mesh->BindIABuffer(context);

		parameter.SetAddress("gTransforms", transform->GetGPUVirtualAddress());
		parameter.SetAddress("gMaterials",  material->GetGPUVirtualAddress());
		//!< todo: materialをConstantBufferに変更する
		 
		core->BindGraphicsBuffer(FRenderCoreGeometry::Type::Deferred_MeshVS, context, parameter);

		mesh->DrawCall(context, 1);
		//!< todo: インスタンス描画対応

	});

}

void FRenderPassDeferredBase::PassSkinnedMesh(const DirectXQueueContext* context, const Config& config) {

	auto core = FRenderCore::GetInstance()->GetGeometry();
	core->SetPipeline(FRenderCoreGeometry::Type::Deferred_MeshVS, context, config.buffer->GetSize());

	// common parameterの設定
	DxObject::BindBufferDesc parameter = {};
	parameter.SetAddress("gCamera", config.camera->GetGPUVirtualAddress());

	sComponentStorage->ForEachActive<SkinnedMeshRendererComponent>([&](SkinnedMeshRendererComponent* component) {

		auto transform = component->GetTransform();

		auto material = component->GetMaterial();
		
		if (material->GetMode() != UAssetMaterial::Mode::Opaque) {
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
