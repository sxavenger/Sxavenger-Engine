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
#include <Engine/Component/Components/Transform/RectTransformComponent.h>
#include <Engine/Component/Components/TextRenderer/TextRendererComponent.h>
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
	context->SetCondition([this]() { return SxavengerSystem::IsPressKey(KeyId::KEY_ESCAPE); });
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
		light->SetIntensity(3.0f);
	}

	{ //!< performance
		performance_ = std::make_unique<PerformanceActor>();
		performance_->Init({ 1190.0f, 0.0f });
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

	{ //!< cubes
		cubes_ = std::make_unique<EmissiveCubes>();
		cubes_->Load();
		cubes_->Awake();
		cubes_->Start();
	}

	camera_->SetSubject(player_->GetComponent<TransformComponent>());
	player_->SetCamera(camera_.get());
	cubes_->SetCamera(camera_.get());

	{
		demoText_ = std::make_unique<MonoBehaviour>();
		demoText_->SetName("demo text");
		auto& transform = demoText_->AddComponent<RectTransformComponent>()->GetTransform();
		transform.scale     = { 400.0f, 50.0f };
		transform.pivot     = { 0.0f, 0.0f };
		transform.translate = { 16.0f, 0.0f };

		auto text = demoText_->AddComponent<TextRendererComponent>();
		text->SetFont(sUContentStorage->Import<UContentFont>("assets/font/MPLUSRounded1c-Regular.ttf")->GetId());
		text->SetSize(32.0f);
		text->SetText(L"Sxavenger Engine Demo : Sponza");
	}

	{
		text_ = std::make_unique<MonoBehaviour>();
		text_->SetName("text");

		auto& transform = text_->AddComponent<RectTransformComponent>()->GetTransform();
		transform.scale     = { 400.0f, 200.0f };
		transform.pivot     = { 0.0f, 1.0f };
		transform.translate = { 16.0f, 760.0f };

		auto text = text_->AddComponent<TextRendererComponent>();
		text->SetFont(sUContentStorage->Import<UContentFont>("assets/font/MPLUSRounded1c-Regular.ttf")->GetId());
		text->SetSize(20.0f);

		std::wstring t = L"";
		t += L"[WASD]            : いどう\n";
		t += L"[LSHIFT] + [WASD] : ダッシュ\n";
		t += L"[みぎクリック]      : してんいどう\n";
		t += L"[F]               : Cubeをもつ\n";
		t += L"[L]               : ライトをつける/けす\n";
		t += L"[P]               : パストレーシングモード\n";
		t += L"[ESC]             : ゲームしゅうりょう\n";

		text->SetText(t);
	}
	

	sSceneObjects->InputJsonFromFilepath("assets/scene/collision_sponza.scene");

}

void DemoGameLoop::TermGame() {
}

void DemoGameLoop::UpdateGame() {

	//-----------------------------------------------------------------------------------------
	// GameLogic Update
	//-----------------------------------------------------------------------------------------

	if (SxavengerSystem::IsTriggerKey(KeyId::KEY_P)) {
		auto& config = FMainRender::GetInstance()->GetConfig();
		config.isEnableIndirectLighting = !config.isEnableIndirectLighting;
	}

	performance_->Update();

	player_->Update();
	camera_->Update();

	cubes_->Update();


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
