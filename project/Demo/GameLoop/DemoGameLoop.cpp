#include "DemoGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Component/Components/Light/Environment/SkyLightComponent.h>
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/Components/Collider/CollisionManager.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/Module/Scene/SceneObjects.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DemoGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DemoGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() { InitGame(); });
	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() { TermGame(); });
	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() {
		if (auto developer = sEditorEngine->GetEditor<DevelopEditor>()) {
			if (!developer->IsProcessRequired()) {
				return;
			}
		}

		UpdateGame();
	});
	context->SetProcess(GameLoop::Process::Render, std::nullopt, [this]() { DrawGame(); });

	context->SetCondition([this]() { return !SxavengerSystem::ProcessMessage(); });
}

void DemoGameLoop::Term() {
}

void DemoGameLoop::InitGame() {

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"Sxavenger Engine Demo").lock();
	main_->SetIcon("packages/icon/SxavengerEngineIcon.ico", { 32, 32 });

	SetCollisionCallback();

	{ //!< skylight
		skylight_ = std::make_unique<MonoBehaviour>();
		skylight_->SetName("skylight");
		auto light = skylight_->AddComponent<SkyLightComponent>();
		light->SetIrradiance(sUContentStorage->Import<UContentTexture>("assets/textures/textureCube/sky_irradiance.dds")->GetId());
		light->SetRadiance(sUContentStorage->Import<UContentTexture>("assets/textures/textureCube/sky_radiance.dds")->GetId());
		light->SetEnvironment(sUContentStorage->Import<UContentTexture>("assets/textures/textureCube/sky_environment.dds")->GetId());
		light->SetIntensity(0.3f);
	}

	{ //!< performance
		performance_ = std::make_unique<PerformanceActor>();
		performance_->Init({ 8.0f, 8.0f });
	}

	{ //!< camera
		camera_ = std::make_unique<PerspectiveCamera>();
		camera_->Load();
		camera_->Awake();
		camera_->Start();
	}

	{ //!< player
		player_ = std::make_unique<Player>();
		player_->Load();
		player_->Awake();
		player_->Start();
	}

	camera_->SetSubject(player_->GetComponent<TransformComponent>());
	player_->SetCamera(camera_.get());

	sSceneObjects->InputJsonFromFilepath("assets/scene/collision_sponza.scene");

	//auto& config = FMainRender::GetInstance()->GetConfig();
	//config.isEnableIndirectLighting = true;

}

void DemoGameLoop::TermGame() {
}

void DemoGameLoop::UpdateGame() {

	//-----------------------------------------------------------------------------------------
	// GameLogic Update
	//-----------------------------------------------------------------------------------------

	performance_->Update();

	player_->Update();
	camera_->Update();

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

void DemoGameLoop::DrawGame() {

	FMainRender::GetInstance()->Render(SxavengerSystem::GetDirectQueueContext(), main_.get());

	main_->BeginRenderWindow(SxavengerSystem::GetDirectQueueContext());
	main_->ClearWindow(SxavengerSystem::GetDirectQueueContext());


	FMainRender::GetInstance()->PresentMain(SxavengerSystem::GetDirectQueueContext());
	SxavengerSystem::RenderImGui();
	
	main_->EndRenderWindow(SxavengerSystem::GetDirectQueueContext());
}

void DemoGameLoop::SetCollisionCallback() {

	sCollisionManager->SetOnCollisionFunctionStay(
		"Player", "Wall",
		[](_MAYBE_UNUSED ColliderComponent* const player, _MAYBE_UNUSED ColliderComponent* const wall) {
		CollisionHelper::PushBackAABB(player, wall);
	});

	sCollisionManager->SetOnCollisionFunctionStay(
		"Cube", "Wall",
		[](_MAYBE_UNUSED ColliderComponent* const cube, _MAYBE_UNUSED ColliderComponent* const wall) {
			CollisionHelper::PushBackAABB(cube, wall);
	});

}
