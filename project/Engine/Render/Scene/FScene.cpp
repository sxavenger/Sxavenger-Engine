#include "FScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FScene::Init() {
	// TLASの初期化
	topLevelAS_.Init(SxavengerSystem::GetDxDevice());

	// stateobjectの初期化
	{ //!< global root signatureの生成
		DxrObject::GlobalRootSignatureDesc desc = {};
		//* textures
		desc.SetHandleUAV(0, 0, 1); //!< color
		desc.SetHandleUAV(1, 1, 1); //!< depth

		//* camera
		desc.SetVirtualCBV(2, 0, 1); //!< camera

		//* scene
		desc.SetVirtualSRV(3, 0, 1); //!< scene

		stateObjectContext_.CreateRootSignature(SxavengerSystem::GetDxDevice(), desc);
	}

	{ //!< state objectの生成
		DxrObject::StateObjectDesc desc = {};
		desc.AddExport(FRenderCore::GetInstance()->GetRaytracing()->GetExportGroup(FRenderCoreRaytracing::RaygenerationExportType::Default));
		desc.AddExport(FRenderCore::GetInstance()->GetRaytracing()->GetExportGroup(FRenderCoreRaytracing::MissExportType::Default));
		desc.AddExport(FRenderCore::GetInstance()->GetRaytracing()->GetExportGroup(FRenderCoreRaytracing::HitgroupExportType::Mesh));

		// 仮paraemter
		desc.SetAttributeStride(sizeof(float) * 2);
		desc.SetPayloadStride(sizeof(float) * 5);
		desc.SetMaxRecursionDepth(4);

		stateObjectContext_.CreateStateObject(SxavengerSystem::GetDxDevice(), std::move(desc));
	}

	
}

void FScene::SetupTopLevelAS(const DirectXThreadContext* context) {
	topLevelAS_.BeginSetupInstance();

	sComponentStorage->ForEach<MeshRendererComponent>([&](MeshRendererComponent* component) {
		if (!component->IsView() || !component->IsActive()) {
			return;
		}

		// 透過の場合はスキップ
		if (component->GetMaterial()->GetMode() != Material::Mode::Opaque) {
			return;
		}

		component->GetMesh()->CreateBottomLevelAS(context);

		DxrObject::TopLevelAS::Instance instance = {};
		instance.flag          = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		instance.bottomLevelAS = component->GetMesh()->GetBottomLevelAS().bottomLevelAS.get();
		instance.mat           = component->GetTransform()->GetMatrix();
		instance.instanceId    = 0;

		//* raytracing export group
		instance.expt = FRenderCore::GetInstance()->GetRaytracing()->GetExportGroup(FRenderCoreRaytracing::HitgroupExportType::Mesh);
		instance.parameter.SetAddress(0, component->GetMesh()->GetVertex()->GetGPUVirtualAddress());
		instance.parameter.SetAddress(1, component->GetMesh()->GetIndex()->GetGPUVirtualAddress());
		instance.parameter.SetAddress(2, component->GetMaterial()->GetGPUVirtualAddress());

		topLevelAS_.AddInstance(instance);
	});

	sComponentStorage->ForEach<SkinnedMeshRendererComponent>([&](SkinnedMeshRendererComponent* component) {
		if (!component->IsView()) {
			return;
		}

		// 透過の場合はスキップ
		if (component->GetMaterial()->GetMode() != Material::Mode::Opaque) {
			return;
		}

		component->GetMesh().UpdateBottomLevelAS(context);

		DxrObject::TopLevelAS::Instance instance = {};
		instance.flag          = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		instance.bottomLevelAS = component->GetMesh().GetBottomLevelAS();
		instance.mat           = component->GetTransform()->GetMatrix();

		//* raytracing export group todo...
		//instance.expt = FRenderCore::GetInstance()->GetRaytracing()->GetExportGroup(FRenderCoreRaytracing::HitgroupExportType::Mesh);
		//instance.parameter.SetAddress(0, component->GetMesh().);
		//instance.parameter.SetAddress(1, component->GetMesh()->GetIndex()->GetGPUVirtualAddress());
		//instance.parameter.SetAddress(2, component->GetMaterial()->GetGPUVirtualAddress());

		topLevelAS_.AddInstance(instance);
	});

	topLevelAS_.EndSetupInstance(SxavengerSystem::GetDxDevice(), context->GetDxCommand());
	context->TransitionAllocator();
}

void FScene::SetupStateObject() {
	// SetupTopLevelAS()に設定
	stateObjectContext_.UpdateShaderTable(SxavengerSystem::GetDxDevice(), &topLevelAS_);
}
