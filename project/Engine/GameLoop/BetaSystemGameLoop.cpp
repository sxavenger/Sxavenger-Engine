#include "BetaSystemGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/SxavengerContent.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/Components/Collider/CollisionManager.h>
#include <Engine/Component/Components/SpriteRenderer/SpriteRendererComponent.h>
#include <Engine/Component/Components/Particle/ParticleComponent.h>
#include <Engine/Component/Components/PostProcessLayer/PostProcessLayerComponent.h>
#include <Engine/Component/Components/Transform/RectTransformComponent.h>
#include <Engine/Component/Components/TextRenderer/TextRendererComponent.h>
#include <Engine/Component/Components/Audio/AudioSourceComponent.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Render/FRenderCore.h>
#include <Engine/Content/InputGeometry/InputPrimitiveHelper.h>

#include "Engine/Component/Components/Light/Environment/SkyLightComponent.h"
#include "Engine/Component/Components/Light/Environment/SkyAtmosphereComponent.h"

#include "Engine/Preview/Content/ContentStorage.h"


////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BetaSystemGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() { InitSystem(); });
	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() { TermSystem(); });
	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() {
		if (auto develop = sEditorEngine->GetEditor<DevelopEditor>()) {
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

	camera_ = std::make_unique<ControllableCameraActor>();
	camera_->Init();
	camera_->GetComponent<CameraComponent>()->SetTag(CameraComponent::Tag::Game);

	skylight_ = std::make_unique<MonoBehaviour>();
	auto light = skylight_->AddComponent<SkyLightComponent>();
	light->SetIrradiance(sContentStorage->Import<ContentTexture>("assets/textures/textureCube/sky_irradiance.dds")->GetId());
	light->SetRadiance(sContentStorage->Import<ContentTexture>("assets/textures/textureCube/sky_radiance.dds")->GetId());
	light->SetEnvironment(sContentStorage->Import<ContentTexture>("assets/textures/textureCube/sky_environment.dds")->GetId());

	behaviour_ = std::make_unique<MonoBehaviour>();
	behaviour_->AddComponent<TransformComponent>();
	behaviour_->AddComponent<SkyAtmosphereComponent>();

	performance_ = std::make_unique<PerformanceActor>();
	performance_->Init();

}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {

	//-----------------------------------------------------------------------------------------
	// Update
	//-----------------------------------------------------------------------------------------

	//atmosphere_->Update();
	camera_->Update();

	behaviour_->GetComponent<SkyAtmosphereComponent>()->UpdateTransmittance(SxavengerSystem::GetDirectQueueContext());
	behaviour_->GetComponent<SkyAtmosphereComponent>()->UpdateMultipleScattering(SxavengerSystem::GetDirectQueueContext());
	behaviour_->GetComponent<SkyAtmosphereComponent>()->UpdateMultipleScattering(SxavengerSystem::GetDirectQueueContext());
	behaviour_->GetComponent<SkyAtmosphereComponent>()->UpdateSkyCube(SxavengerSystem::GetDirectQueueContext());

	performance_->Update();

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

	//* test *//
	

}

void BetaSystemGameLoop::DrawSystem() {

	FMainRender::GetInstance()->Render(SxavengerSystem::GetDirectQueueContext(), main_.get());

	main_->BeginRenderWindow(SxavengerSystem::GetDirectQueueContext());
	main_->ClearWindow(SxavengerSystem::GetDirectQueueContext());
	 
	FMainRender::GetInstance()->PresentMain(SxavengerSystem::GetDirectQueueContext());
	SxavengerSystem::RenderImGui();

	main_->EndRenderWindow(SxavengerSystem::GetDirectQueueContext());

	//* test *//

}
