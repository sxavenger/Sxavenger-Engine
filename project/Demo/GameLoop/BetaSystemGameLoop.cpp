#include "BetaSystemGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/Components/Collider/CollisionManager.h>
#include <Engine/Component/Components/SpriteRenderer/SpriteRendererComponent.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Render/FRenderCore.h>

#include "Engine/Component/Components/Light/SkyLightComponent.h"

#include "Engine/System/Runtime/Thread/AsyncThread.h"

////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BetaSystemGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() { InitSystem(); });
	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() { TermSystem(); });
	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() {
		if (auto develop = sEditorEngine->TryGetEditor<DevelopEditor>()) {
			if (!develop->IsProcessRequired()) {
				return;
			}
		}

		UpdateSystem();
	});
	context->SetProcess(GameLoop::Process::Render, std::nullopt, [this]() { DrawSystem(); });

	context->SetCondition(
		[this]() { return !SxavengerSystem::ProcessMessage(); }
	);
}

void BetaSystemGameLoop::Term() {
}

void BetaSystemGameLoop::InitSystem() {

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"sxavenger engine beta window").lock();
	main_->SetIcon("packages/icon/SxavengerEngineIcon.ico", { 32, 32 });

	camera_ = ComponentHelper::CreateCameraMonoBehaviour();

	mesh_   = ComponentHelper::CreateStaticNodeModelBehaviour("assets/models/PBR_Sphere_Test/model/PBR_Sphere.gltf");
	//mesh_   = ComponentHelper::CreateStaticNodeModelBehaviour("assets/models/sponza/NewSponza_Main_glTF_003.gltf");
	//mesh_   = ComponentHelper::CreateStaticNodeModelBehaviour("assets/models/bricks/bricks.obj");
	//mesh_   = ComponentHelper::CreateStaticNodeModelBehaviour("assets/models/normal_sphere.obj");

	//foundation_ = ComponentHelper::CreateStaticModelBehaviour("assets/models/foundation.gltf");
	
	light_  = ComponentHelper::CreatePointLightMonoBehaviour();
	//light_  = ComponentHelper::CreateDirectionalLightMonoBehaviour();

	
	/*AssetObserver<AssetTexture> diffuse  = SxavengerAsset::TryImport<AssetTexture>("assets/textures/diffuseHDR.dds");
	AssetObserver<AssetTexture> specular = SxavengerAsset::TryImport<AssetTexture>("assets/textures/specularHDR.dds");*/

	RunTimeTracker runtime;
	runtime.Begin();

	//skyAtmosphere_.Create({ 1024, 1024 });
	//skyAtmosphere_.Update(SxavengerSystem::GetMainThreadContext());

	runtime.End();
	LogRuntime(std::format("[atmosphere runtime]: {}s", runtime.GetDeltaTime<TimeUnit::second>().time));

	AssetObserver<AssetTexture> env = SxavengerAsset::TryImport<AssetTexture>("assets/textures/EnvHDR.dds");

	runtime.Begin();

	environmentMap_.Create(env.WaitGet()->GetSize());
	environmentMap_.Dispatch(SxavengerSystem::GetMainThreadContext(), env.WaitGet()->GetGPUHandleSRV());

	runtime.End();
	LogRuntime(std::format("[environment runtime]: {}s", runtime.GetDeltaTime<TimeUnit::second>().time));

	SxavengerSystem::ExecuteAllAllocator();

	skyAtmosphere_.Create({ 1024, 1024 });
	skyAtmosphere_.Update(SxavengerSystem::GetMainThreadContext());

	skylight_ = ComponentHelper::CreateMonoBehaviour();
	skylight_->SetName("sky light");
	skylight_->AddComponent<SkyLightComponent>();

	//skylight_->GetComponent<SkyLightComponent>()->GetDiffuseParameter().SetTexture(diffuse);
	//skylight_->GetComponent<SkyLightComponent>()->GetSpecularParameter().SetTexture(specular);

	skylight_->GetComponent<SkyLightComponent>()->GetDiffuseParameter().SetTexture(skyAtmosphere_.GetIrradiance().descriptorSRV.GetIndex());
	skylight_->GetComponent<SkyLightComponent>()->GetSpecularParameter().SetTexture(skyAtmosphere_.GetRadiance().descriptorSRV.GetIndex(), skyAtmosphere_.GetRadiance().kMiplevels);

	//skylight_->GetComponent<SkyLightComponent>()->GetDiffuseParameter().SetTexture(environmentMap_.GetIrradianceIndex());
	//skylight_->GetComponent<SkyLightComponent>()->GetSpecularParameter().SetTexture(environmentMap_.GetRadianceIndex(), environmentMap_.GetRadianceMiplevel());
}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {

	//-----------------------------------------------------------------------------------------
	// Update
	//-----------------------------------------------------------------------------------------

	//skyAtmosphere_.Update(SxavengerSystem::GetMainThreadContext());

	//-----------------------------------------------------------------------------------------
	// SystemUpdate...?
	//-----------------------------------------------------------------------------------------

	ComponentHelper::UpdateTransform();
	// todo: engine側のgameloopに移動.

	sCollisionManager->CheckCollision();
	// todo: engine側のgameloopに移動.

	//-----------------------------------------------------------------------------------------
	// LateUpdate
	//-----------------------------------------------------------------------------------------

	//-----------------------------------------------------------------------------------------
	// final Update...?
	//-----------------------------------------------------------------------------------------

	ComponentHelper::UpdateSkinning();
	// todo: engine側のgameloopに移動.

}

void BetaSystemGameLoop::DrawSystem() {

	FMainRender::GetInstance()->Render(SxavengerSystem::GetMainThreadContext());

	main_->BeginRendering();
	main_->ClearWindow();

	FMainRender::GetInstance()->PresentMain(SxavengerSystem::GetMainThreadContext());
	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
