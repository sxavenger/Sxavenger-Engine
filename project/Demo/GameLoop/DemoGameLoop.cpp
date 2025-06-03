//[DemoGameLoop.cpp]
#include "DemoGameLoop.h"

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

	stage_ = std::make_unique<Stage>();
	stage_->Load();
	stage_->Awake();
	stage_->Start();

	player_ = std::make_unique<Player>();
	player_->Load();
	player_->Awake();
	player_->Start();

	smoke_ = std::make_unique<SmokeParticle>();
	smoke_->Load();
	smoke_->Awake();
	smoke_->Start();

	rain_ = std::make_unique<RainParticle>();
	rain_->Load();
	rain_->Awake();
	rain_->Start();

	lead_ = std::make_unique<LeadParticle>();
	lead_->Load();
	lead_->Awake();
	lead_->Start();

	sampleLight_ = ComponentHelper::CreateDirectionalLightMonoBehaviour();
	sampleLight_->GetComponent<TransformComponent>()->rotate *= Quaternion::ToQuaternion({ kPi / 4.0f, 0.0f, 0.0f });

	skylight_ = std::make_unique<AtmosphereActor>();
	skylight_->Init({ 1024, 1024 });

	SetCollisionCallback();
}

void DemoGameLoop::TermGame() {
}

void DemoGameLoop::UpdateGame() {

	//-----------------------------------------------------------------------------------------
	// GameLogic Update
	//-----------------------------------------------------------------------------------------

	skylight_->Update();

	player_->Update();

	smoke_->Update();
	rain_->Update();
	lead_->Update();

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

	FMainRender::GetInstance()->Render(SxavengerSystem::GetMainThreadContext());

	main_->BeginRendering();
	main_->ClearWindow();


	FMainRender::GetInstance()->PresentMain(SxavengerSystem::GetMainThreadContext());
	SxavengerSystem::RenderImGui();
	
	main_->EndRendering();
}

void DemoGameLoop::SetCollisionCallback() {
	{
		CollisionCallbackCollection::OnCollisionCallbacks callback = {};
		callback.enter = [](_MAYBE_UNUSED ColliderComponent* const colliderA, _MAYBE_UNUSED ColliderComponent* const colliderB) {
			CommentRuntime("on collision enter.");

			auto player = static_cast<Player*>(colliderA->GetBehaviour());
			player->SetCameraTarget(colliderB->GetTransform(), 4.0f);

			auto particle = static_cast<LeadParticle*>(colliderB->GetBehaviour());
			particle->SetTarget({ Random::UniformDistribution(-12.0f, 12.0f), 1.5f, Random::UniformDistribution(-12.0f, 12.0f) });
		};

		sCollisionManager->SetOnCollisionFunctions("player", "lead particle", callback);
	}
	
}
