#include "FScene.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Core/FRenderCore.h"

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

	 //!< light containerの初期化
	directionalLightContainer_.Init(System::GetDxDevice());
	pointLightContainer_.Init(System::GetDxDevice());
	spotLightContainer_.Init(System::GetDxDevice());
	
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

		if (material->GetMode() == AssetMaterial::Mode::Translucent) {
			return; //!< 透明マテリアルはTLASに登録しない
		}

		mesh->Update(context); //!< meshの更新

		// instanceの設定
		DxrObject::TopLevelAS::Instance instance = {};

		instance.flag          = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		instance.bottomLevelAS = mesh->GetInputMesh().GetBottomLevelAS().Get();
		instance.mat           = component->RequireTransform()->GetMatrix();
		instance.instanceMask  = component->GetMask();
		instance.instanceId    = NULL;

		//* ExportGroupの設定
		instance.name = magic_enum::enum_name(material->GetMode());
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

		if (material->GetMode() == AssetMaterial::Mode::Translucent) {
			return; //!< 透明マテリアルはTLASに登録しない
		}

		component->Update(context); //!< meshの更新

		// instanceの設定
		DxrObject::TopLevelAS::Instance instance = {};

		instance.flag          = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		instance.bottomLevelAS = component->GetBottomLevelAS();
		instance.mat           = component->RequireTransform()->GetMatrix();
		instance.instanceMask  = component->GetMask();
		instance.instanceId    = NULL;

		//* ExportGroupの設定
		instance.name = magic_enum::enum_name(material->GetMode());
		instance.parameter.SetAddress(0, component->GetInputVertex()->GetGPUVirtualAddress());
		instance.parameter.SetAddress(1, component->GetInputIndex()->GetGPUVirtualAddress());
		instance.parameter.SetAddress(2, material->GetGPUVirtualAddress());

		topLevelAS_.AddInstance(instance);
	});

	topLevelAS_.EndSetupInstance(System::GetDxDevice(), context->GetDxCommand());
}

void FScene::SetupStateObject() {
	// TopLevelASに設定
	//FRenderCore::GetInstance()->GetRestir()->UpdateShaderTable(&topLevelAS_);
	//FRenderCore::GetInstance()->GetProbe()->UpdateShaderTable(&topLevelAS_); //!< HACK
	//FRenderCore::GetInstance()->GetReflection()->UpdateShaderTable(&topLevelAS_); //!< HACK
}

void FScene::SetupLightContainer() {
	SetupDirectionalLight();
	SetupPointLight();
	SetupSpotLight();
}

void FScene::SetupDirectionalLight() {

	uint32_t count = static_cast<uint32_t>(sComponentStorage->GetActiveComponentCount<DirectionalLightComponent>());
	directionalLightContainer_.Resize(System::GetDxDevice(), count);

	if (count == 0) {
		return;
	}

	size_t index = 0;

	sComponentStorage->ForEachActive<DirectionalLightComponent>([&](DirectionalLightComponent* component) {
		directionalLightContainer_.transforms.At(index) = component->RequireTransform()->GetTransformationMatrix();
		directionalLightContainer_.parameters.At(index) = component->GetParameter();
		index++;
	});

}

void FScene::SetupPointLight() {

	uint32_t count = static_cast<uint32_t>(sComponentStorage->GetActiveComponentCount<PointLightComponent>());
	pointLightContainer_.Resize(System::GetDxDevice(), count);

	if (count == 0) {
		return;
	}

	size_t index = 0;

	sComponentStorage->ForEachActive<PointLightComponent>([&](PointLightComponent* component) {
		pointLightContainer_.transforms.At(index) = component->RequireTransform()->GetTransformationMatrix();
		pointLightContainer_.parameters.At(index) = component->GetParameter();
		index++;
	});


}

void FScene::SetupSpotLight() {

	uint32_t count = static_cast<uint32_t>(sComponentStorage->GetActiveComponentCount<SpotLightComponent>());
	spotLightContainer_.Resize(System::GetDxDevice(), count);

	if (count == 0) {
		return;
	}

	size_t index = 0;

	sComponentStorage->ForEachActive<SpotLightComponent>([&](SpotLightComponent* component) {
		spotLightContainer_.transforms.At(index) = component->RequireTransform()->GetTransformationMatrix();
		spotLightContainer_.parameters.At(index) = component->GetParameter();
		index++;
	});

}
