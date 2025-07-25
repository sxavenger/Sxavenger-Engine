#include "FScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Render/FRenderCore.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FScene::Init() {
	// TLASの初期化
	topLevelAS_.Init(SxavengerSystem::GetDxDevice());

	// path tracing stateobjectの初期化
	{ //!< global root signatureの生成
		DxrObject::GlobalRootSignatureDesc desc = {};
		//* textures
		desc.SetHandleUAV(0, 0, 1); //!< color
		desc.SetHandleUAV(1, 1, 1); //!< normal
		desc.SetHandleUAV(2, 2, 1); //!< arm
		desc.SetHandleUAV(3, 3, 1); //!< position
		desc.SetHandleUAV(4, 4, 1); //!< depth

		//* camera
		desc.SetVirtualCBV(5, 0, 1); //!< camera

		//* scene
		desc.SetVirtualSRV(6, 0, 1); //!< scene

		//* reservoir
		desc.SetVirtualCBV(7, 1, 1); //!< reservoir

		//* light
		//# Directional Light
		desc.SetVirtualCBV(8, 2, 1);  //!< light count
		desc.SetVirtualSRV(9, 1, 1);  //!< light transforms
		desc.SetVirtualSRV(10, 2, 1); //!< light parameters
		desc.SetVirtualSRV(11, 3, 1); //!< light shadow parameters

		//# Point Light
		desc.SetVirtualCBV(12, 3, 1); //!< light count
		desc.SetVirtualSRV(13, 4, 1); //!< light transforms
		desc.SetVirtualSRV(14, 5, 1); //!< light parameters
		desc.SetVirtualSRV(15, 6, 1); //!< light shadow parameters

		stateObjectContext_.CreateRootSignature(SxavengerSystem::GetDxDevice(), desc);
	}

	{ //!< state objectの生成
		DxrObject::StateObjectDesc desc = {};
		desc.AddExport(FRenderCore::GetInstance()->GetPathtracing()->GetExportGroup(FRenderCorePathtracing::RaygenerationExportType::Default));
		desc.AddExport(FRenderCore::GetInstance()->GetPathtracing()->GetExportGroup(FRenderCorePathtracing::MissExportType::Default));
		desc.AddExport(FRenderCore::GetInstance()->GetPathtracing()->GetExportGroup(FRenderCorePathtracing::HitgroupExportType::Mesh));
		desc.AddExport(FRenderCore::GetInstance()->GetPathtracing()->GetExportGroup(FRenderCorePathtracing::HitgroupExportType::Emissive));

		// 仮paraemter
		desc.SetAttributeStride(sizeof(float) * 2);
		desc.SetPayloadStride(76);
		desc.SetMaxRecursionDepth(6);

		stateObjectContext_.CreateStateObject(SxavengerSystem::GetDxDevice(), std::move(desc));
	}

	{ //!< light containerの初期化
		directionalLightCount_ = std::make_unique<DxObject::DimensionBuffer<uint32_t>>();
		directionalLightCount_->Create(SxavengerSystem::GetDxDevice(), 1);
		directionalLightTransforms_   = std::make_unique<DxObject::DimensionBuffer<TransformationMatrix>>();
		directionalLightParams_       = std::make_unique<DxObject::DimensionBuffer<DirectionalLightComponent::Parameter>>();
		directionalLightShadowParams_ = std::make_unique<DxObject::DimensionBuffer<DirectionalLightComponent::InlineShadow>>();

		pointLightCount_ = std::make_unique<DxObject::DimensionBuffer<uint32_t>>();
		pointLightCount_->Create(SxavengerSystem::GetDxDevice(), 1);
		pointLightTransforms_   = std::make_unique<DxObject::DimensionBuffer<TransformationMatrix>>();
		pointLightParams_       = std::make_unique<DxObject::DimensionBuffer<PointLightComponent::Parameter>>();
		pointLightShadowParams_ = std::make_unique<DxObject::DimensionBuffer<PointLightComponent::InlineShadow>>();

	}
	
}

void FScene::SetupTopLevelAS(const DirectXQueueContext* context) {
	topLevelAS_.BeginSetupInstance();

	sComponentStorage->ForEachActive<MeshRendererComponent>([&](MeshRendererComponent* component) {
		
		DxrObject::TopLevelAS::Instance instance = {};

		// instanceの設定
		switch (component->GetMaterial()->GetMode()) {
			case Material::Mode::Opaque:
				component->GetMesh()->CreateBottomLevelAS(context);

				//* instance設定
				instance.flag          = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
				instance.bottomLevelAS = component->GetMesh()->GetBottomLevelAS().bottomLevelAS.get();
				instance.mat           = component->GetTransform()->GetMatrix();
				instance.instanceId    = 0;

				//* ExportGroupの設定
				instance.expt = FRenderCore::GetInstance()->GetPathtracing()->GetExportGroup(FRenderCorePathtracing::HitgroupExportType::Mesh);
				instance.parameter.SetAddress(0, component->GetMesh()->GetVertex()->GetGPUVirtualAddress());
				instance.parameter.SetAddress(1, component->GetMesh()->GetIndex()->GetGPUVirtualAddress());
				instance.parameter.SetAddress(2, component->GetMaterial()->GetGPUVirtualAddress());
				break;
				

			case Material::Mode::Emissive:
				component->GetMesh()->CreateBottomLevelAS(context);

				//* instance設定
				instance.flag          = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
				instance.bottomLevelAS = component->GetMesh()->GetBottomLevelAS().bottomLevelAS.get();
				instance.mat           = component->GetTransform()->GetMatrix();
				instance.instanceId    = 0;

				//* ExportGroupの設定
				instance.expt = FRenderCore::GetInstance()->GetPathtracing()->GetExportGroup(FRenderCorePathtracing::HitgroupExportType::Emissive);
				instance.parameter.SetAddress(0, component->GetMesh()->GetVertex()->GetGPUVirtualAddress());
				instance.parameter.SetAddress(1, component->GetMesh()->GetIndex()->GetGPUVirtualAddress());
				instance.parameter.SetAddress(2, component->GetMaterial()->GetGPUVirtualAddress());
				break;

			default:
				Logger::WarningRuntime("warning | [FScene] SetupTopLevelAS", "MeshRendererComponent has unsupported material mode.");
				return;
		}

		topLevelAS_.AddInstance(instance);
	});

	sComponentStorage->ForEachActive<SkinnedMeshRendererComponent>([&](SkinnedMeshRendererComponent* component) {

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
		instance.expt = FRenderCore::GetInstance()->GetPathtracing()->GetExportGroup(FRenderCorePathtracing::HitgroupExportType::Mesh);
		instance.parameter.SetAddress(0, component->GetMesh().vertex->GetGPUVirtualAddress());
		instance.parameter.SetAddress(1, component->GetReferenceMesh()->input.GetIndex()->GetGPUVirtualAddress());
		instance.parameter.SetAddress(2, component->GetMaterial()->GetGPUVirtualAddress());

		topLevelAS_.AddInstance(instance);
	});

	topLevelAS_.EndSetupInstance(SxavengerSystem::GetDxDevice(), context->GetDxCommand());
	context->TransitionAllocator();
}

void FScene::SetupStateObject() {
	// SetupTopLevelAS()に設定
	stateObjectContext_.UpdateShaderTable(SxavengerSystem::GetDxDevice(), &topLevelAS_);
}

void FScene::SetupLightContainer() {
	SetupDirectionalLight();
	SetupPointLight();
}

void FScene::SetupDirectionalLight() {

	uint32_t count = static_cast<uint32_t>(sComponentStorage->GetActiveComponentCount<DirectionalLightComponent>());

	directionalLightCount_->At(0) = count;

	if (count == 0) {
		return;
	}

	if (directionalLightTransforms_->GetSize() < count) {
		directionalLightTransforms_->Create(SxavengerSystem::GetDxDevice(), count);
	}

	if (directionalLightParams_->GetSize() < count) {
		directionalLightParams_->Create(SxavengerSystem::GetDxDevice(), count);
	}

	if (directionalLightShadowParams_->GetSize() < count) {
		directionalLightShadowParams_->Create(SxavengerSystem::GetDxDevice(), count);
	}

	size_t index = 0;

	sComponentStorage->ForEachActive<DirectionalLightComponent>([&](DirectionalLightComponent* component) {
		directionalLightTransforms_->At(index)   = component->RequireTransform()->GetTransformationMatrix();
		directionalLightParams_->At(index)       = component->GetParameter();
		directionalLightShadowParams_->At(index) = component->GetShadowParameter();

		index++;
	});

}

void FScene::SetupPointLight() {

	uint32_t count = static_cast<uint32_t>(sComponentStorage->GetActiveComponentCount<PointLightComponent>());

	pointLightCount_->At(0) = count;

	if (count == 0) {
		return;
	}

	if (pointLightTransforms_->GetSize() < count) {
		pointLightTransforms_->Create(SxavengerSystem::GetDxDevice(), count);
	}

	if (pointLightParams_->GetSize() < count) {
		pointLightParams_->Create(SxavengerSystem::GetDxDevice(), count);
	}

	if (pointLightShadowParams_->GetSize() < count) {
		pointLightShadowParams_->Create(SxavengerSystem::GetDxDevice(), count);
	}

	size_t index = 0;

	sComponentStorage->ForEachActive<PointLightComponent>([&](PointLightComponent* component) {
		pointLightTransforms_->At(index)   = component->RequireTransform()->GetTransformationMatrix();
		pointLightParams_->At(index)       = component->GetParameter();
		pointLightShadowParams_->At(index) = component->GetShadowParameter();

		index++;
	});


}
