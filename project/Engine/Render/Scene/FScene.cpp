#include "FScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FScene::Create() {
	topLevelAS_ = std::make_unique<DxrObject::TopLevelAS>();
}

void FScene::Term() {
	topLevelAS_.reset();
}

void FScene::SetupTopLevelAS(const DirectXThreadContext* context) {
	topLevelAS_->BeginSetupInstance();

	sComponentStorage->ForEach<MeshRendererComponent>([&](MeshRendererComponent* component) {
		// todo: std::execution::parに変更
		if (!component->IsActive()) {
			return;
		}

		// 透過の場合はスキップ
		if (component->GetMaterial()->GetBlendMode() != Material::BlendMode::Opaque) {
			return;
		}

		component->GetMesh()->CreateBottomLevelAS(context);

		DxrObject::TopLevelAS::Instance instance = {};
		instance.flag          = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		instance.bottomLevelAS = component->GetMesh()->GetBottomLevelAS();
		instance.mat           = component->GetTransform()->GetMatrix();
		instance.instanceId    = 0;
		// hack: inline raytracing以外で使う場合はここにExportGroupを設定する

		topLevelAS_->AddInstance(instance);
	});

	topLevelAS_->EndSetupInstance(SxavengerSystem::GetDxDevice(), context->GetDxCommand());
	context->TransitionAllocator();
}
