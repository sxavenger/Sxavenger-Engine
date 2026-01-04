#include "FScene.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderCore.h"

//* engine
#include <Engine/System/Utility/RuntimeLogger.h>
#include <Engine/System/System.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FScene::Init() {
	// TLASの初期化
	topLevelAS_.Init(System::GetDxDevice());

	{ //!< light containerの初期化
		directionalLightCount_ = std::make_unique<DxObject::ConstantBuffer<uint32_t>>();
		directionalLightCount_->Create(System::GetDxDevice());

		directionalLightTransforms_   = std::make_unique<DxObject::DimensionBuffer<TransformationMatrix>>();
		directionalLightTransforms_->Create(System::GetDxDevice(), 1);

		directionalLightParams_       = std::make_unique<DxObject::DimensionBuffer<DirectionalLightComponent::Parameter>>();
		directionalLightParams_->Create(System::GetDxDevice(), 1);

		pointLightCount_ = std::make_unique<DxObject::ConstantBuffer<uint32_t>>();
		pointLightCount_->Create(System::GetDxDevice());

		pointLightTransforms_   = std::make_unique<DxObject::DimensionBuffer<TransformationMatrix>>();
		pointLightTransforms_->Create(System::GetDxDevice(), 1);

		pointLightParams_       = std::make_unique<DxObject::DimensionBuffer<PointLightComponent::Parameter>>();
		pointLightParams_->Create(System::GetDxDevice(), 1);

		spotLightCount_ = std::make_unique<DxObject::ConstantBuffer<uint32_t>>();
		spotLightCount_->Create(System::GetDxDevice());

		spotLightTransforms_ = std::make_unique<DxObject::DimensionBuffer<TransformationMatrix>>();
		spotLightTransforms_->Create(System::GetDxDevice(), 1);

		spotLightParams_ = std::make_unique<DxObject::DimensionBuffer<SpotLightComponent::Parameter>>();
		spotLightParams_->Create(System::GetDxDevice(), 1);

	}
	
}

void FScene::SetupTopLevelAS(const DirectXQueueContext* context) {
	topLevelAS_.BeginSetupInstance();

	sComponentStorage->ForEachActive<MeshRendererComponent>([&](MeshRendererComponent* component) {
		if (!component->IsEnable()) { //!< 不適格component
			return; //!< material or meshが設定されていない場合はスキップ
			// todo: missing material を用意する
		}

		std::shared_ptr<AssetMesh> mesh         = component->GetMesh();
		std::shared_ptr<AssetMaterial> material = component->GetMaterial();

		mesh->Update(context); //!< meshの更新

		FRenderCoreRestir::HitgroupExportType type = {};

		switch (material->GetMode()) {
			case AssetMaterial::Mode::Opaque:
				type = FRenderCoreRestir::HitgroupExportType::Mesh;
				break;

			case AssetMaterial::Mode::Emissive:
				type = FRenderCoreRestir::HitgroupExportType::Emissive;
				break;

			default:
				RuntimeLogger::LogWarning("[FScene]", "MeshRendererComponent has unsupported material mode.");
				return;
		}

		// instanceの設定
		DxrObject::TopLevelAS::Instance instance = {};

		instance.flag          = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		instance.bottomLevelAS = mesh->GetInputMesh().GetBottomLevelAS().Get();
		instance.mat           = component->RequireTransform()->GetMatrix();
		instance.instanceMask  = component->GetMask();
		instance.instanceId    = NULL;

		//* ExportGroupの設定
		instance.expt = FRenderCore::GetInstance()->GetRestir()->GetExportGroup(type);
		instance.parameter.SetAddress(0, mesh->GetInputVertex()->GetGPUVirtualAddress());
		instance.parameter.SetAddress(1, mesh->GetInputIndex()->GetGPUVirtualAddress());
		instance.parameter.SetAddress(2, material->GetGPUVirtualAddress());

		topLevelAS_.AddInstance(instance);
	});

	sComponentStorage->ForEachActive<SkinnedMeshRendererComponent>([&](SkinnedMeshRendererComponent* component) {
		if (!component->IsEnable()) { //!< 不適格component
			return; //!< material or meshが設定されていない場合はスキップ
			// todo: missing material を用意する
		}

		std::shared_ptr<AssetMaterial> material = component->GetMaterial();

		component->Update(context); //!< meshの更新

		FRenderCoreRestir::HitgroupExportType type = {};

		switch (material->GetMode()) {
			case AssetMaterial::Mode::Opaque:
				type = FRenderCoreRestir::HitgroupExportType::Mesh;
				break;

			case AssetMaterial::Mode::Emissive:
				type = FRenderCoreRestir::HitgroupExportType::Emissive;
				break;

			default:
				RuntimeLogger::LogWarning("[FScene]", "MeshRendererComponent has unsupported material mode.");
				return;
		}

		// instanceの設定
		DxrObject::TopLevelAS::Instance instance = {};

		instance.flag          = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		instance.bottomLevelAS = component->GetBottomLevelAS();
		instance.mat           = component->RequireTransform()->GetMatrix();
		instance.instanceMask  = component->GetMask();
		instance.instanceId    = NULL;

		//* ExportGroupの設定
		instance.expt = FRenderCore::GetInstance()->GetRestir()->GetExportGroup(type);
		instance.parameter.SetAddress(0, component->GetInputVertex()->GetGPUVirtualAddress());
		instance.parameter.SetAddress(1, component->GetInputIndex()->GetGPUVirtualAddress());
		instance.parameter.SetAddress(2, material->GetGPUVirtualAddress());

		topLevelAS_.AddInstance(instance);
	});

	topLevelAS_.EndSetupInstance(System::GetDxDevice(), context->GetDxCommand());
}

void FScene::SetupStateObject() {
	// TopLevelASに設定
	FRenderCore::GetInstance()->GetRestir()->UpdateShaderTable(&topLevelAS_);
	FRenderCore::GetInstance()->GetProbe()->UpdateShaderTable(&topLevelAS_); //!< HACK
}

void FScene::SetupLightContainer() {
	SetupDirectionalLight();
	SetupPointLight();
	SetupSpotLight();
}

void FScene::SetupDirectionalLight() {

	uint32_t count = static_cast<uint32_t>(sComponentStorage->GetActiveComponentCount<DirectionalLightComponent>());

	directionalLightCount_->At() = count;

	if (count == 0) {
		return;
	}

	if (directionalLightTransforms_->GetSize() < count) {
		directionalLightTransforms_->Create(System::GetDxDevice(), count);
	}

	if (directionalLightParams_->GetSize() < count) {
		directionalLightParams_->Create(System::GetDxDevice(), count);
	}

	size_t index = 0;

	sComponentStorage->ForEachActive<DirectionalLightComponent>([&](DirectionalLightComponent* component) {
		directionalLightTransforms_->At(index)   = component->RequireTransform()->GetTransformationMatrix();
		directionalLightParams_->At(index)       = component->GetParameter();

		index++;
	});

}

void FScene::SetupPointLight() {

	uint32_t count = static_cast<uint32_t>(sComponentStorage->GetActiveComponentCount<PointLightComponent>());

	pointLightCount_->At() = count;

	if (count == 0) {
		return;
	}

	if (pointLightTransforms_->GetSize() < count) {
		pointLightTransforms_->Create(System::GetDxDevice(), count);
	}

	if (pointLightParams_->GetSize() < count) {
		pointLightParams_->Create(System::GetDxDevice(), count);
	}

	size_t index = 0;

	sComponentStorage->ForEachActive<PointLightComponent>([&](PointLightComponent* component) {
		pointLightTransforms_->At(index)   = component->RequireTransform()->GetTransformationMatrix();
		pointLightParams_->At(index)       = component->GetParameter();

		index++;
	});


}

void FScene::SetupSpotLight() {

	uint32_t count = static_cast<uint32_t>(sComponentStorage->GetActiveComponentCount<SpotLightComponent>());

	spotLightCount_->At() = count;

	if (count == 0) {
		return;
	}

	if (spotLightTransforms_->GetSize() < count) {
		spotLightTransforms_->Create(System::GetDxDevice(), count);
	}

	if (spotLightParams_->GetSize() < count) {
		spotLightParams_->Create(System::GetDxDevice(), count);
	}

	size_t index = 0;

	sComponentStorage->ForEachActive<SpotLightComponent>([&](SpotLightComponent* component) {
		spotLightTransforms_->At(index)   = component->RequireTransform()->GetTransformationMatrix();
		spotLightParams_->At(index)       = component->GetParameter();

		index++;
	});

}
