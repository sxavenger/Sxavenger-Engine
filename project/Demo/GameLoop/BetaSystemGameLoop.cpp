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
#include <Engine/Component/Components/Particle/ParticleComponent.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Render/FRenderCore.h>
#include <Engine/Content/InputGeometry/InputPrimitiveHelper.h>

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

	meshA_   = ComponentHelper::CreateStaticNodeModelBehaviour("assets/models/PBR_Sphere_Test/model/PBR_Sphere.gltf");
	//meshA_   = ComponentHelper::CreateStaticNodeModelBehaviour("assets/models/sponza/NewSponza_Main_glTF_003.gltf");
	//meshA_   = ComponentHelper::CreateStaticNodeModelBehaviour("assets/models/bricks/bricks.obj");
	//meshA_   = ComponentHelper::CreateStaticNodeModelBehaviour("assets/models/normal_sphere.obj");

	meshB_ = ComponentHelper::CreateStaticNodeModelBehaviour("assets/models/chessboard/ABeautifulGame.gltf");

	//foundation_ = ComponentHelper::CreateStaticModelBehaviour("assets/models/foundation.gltf");
	
	light_  = ComponentHelper::CreatePointLightMonoBehaviour();
	//light_  = ComponentHelper::CreateDirectionalLightMonoBehaviour();

	skyAtmosphere_.Create({ 1024, 1024 });
	skyAtmosphere_.Update(SxavengerSystem::GetMainThreadContext());

	skylight_ = ComponentHelper::CreateMonoBehaviour();
	skylight_->SetName("sky light");
	skylight_->AddComponent<SkyLightComponent>();

	skylight_->GetComponent<SkyLightComponent>()->GetDiffuseParameter().SetTexture(skyAtmosphere_.GetIrradiance().descriptorSRV.GetIndex());
	skylight_->GetComponent<SkyLightComponent>()->GetSpecularParameter().SetTexture(skyAtmosphere_.GetRadiance().descriptorSRV.GetIndex(), skyAtmosphere_.GetRadiance().kMiplevels);

	light_->AddComponent<PostProcessLayerComponent>();
	light_->GetComponent<PostProcessLayerComponent>()->AddPostProcess<PostProcessExposure>();
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

	ComponentHelper::UpdateParticle();

}

void BetaSystemGameLoop::DrawSystem() {

	FMainRender::GetInstance()->Render(SxavengerSystem::GetMainThreadContext());

	main_->BeginRendering();
	main_->ClearWindow();

	FMainRender::GetInstance()->PresentMain(SxavengerSystem::GetMainThreadContext());
	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
