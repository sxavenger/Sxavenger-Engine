#include "GameScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Components/Light/Environment/SkyLightComponent.h>
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/Components/Collider/CollisionManager.h>
#include <Engine/Component/Components/Transform/RectTransformComponent.h>
#include <Engine/Component/Components/TextRenderer/TextRendererComponent.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Module/Scene/SceneObjects.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Init() {

	SetCollisionCallback();

	{ //!< sky daily cycle
		skyDailyCycle_ = std::make_unique<SkyDailyCycleActor>();
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

	{ //!< items
		items_ = std::make_unique<CollectibleItems>();
		items_->Load();
		items_->Awake();
		items_->Start();
	}

	camera_->SetSubject(player_->GetComponent<TransformComponent>());
	player_->SetCamera(camera_.get());


	
	sSceneObjects->InputJsonFromFilepath("assets/scene/collision_sponza.scene");

}

void GameScene::Start() {

	{
		demoText_ = std::make_unique<MonoBehaviour>();
		demoText_->SetName("demo text");
		auto& transform = demoText_->AddComponent<RectTransformComponent>()->GetTransform();
		transform.scale     = { 400.0f, 50.0f };
		transform.pivot     = { 0.0f, 0.0f };
		transform.translate = { 16.0f, 0.0f };

		auto text = demoText_->AddComponent<TextRendererComponent>();
		text->SetFont(sContentStorage->Import<ContentFont>("assets/font/MPLUSRounded1c-Regular.ttf")->GetId());
		text->SetSize(32.0f);
		text->SetText(L"Sxavenger Engine Demo");
	}

	{
		text_ = std::make_unique<MonoBehaviour>();
		text_->SetName("text");

		auto& transform = text_->AddComponent<RectTransformComponent>()->GetTransform();
		transform.scale     = { 400.0f, 200.0f };
		transform.pivot     = { 0.0f, 1.0f };
		transform.translate = { 16.0f, 760.0f };

		auto text = text_->AddComponent<TextRendererComponent>();
		text->SetFont(sContentStorage->Import<ContentFont>("assets/font/MPLUSRounded1c-Regular.ttf")->GetId());
		text->SetSize(20.0f);

		std::wstring t = L"";
		t += L"[W][A][S][D]      : いどう\n";
		t += L"[LSHIFT] + [WASD] : ダッシュ\n";
		t += L"[みぎクリック]      : してんいどう\n";
		t += L"[L]               : ライトをつける/けす\n";
		t += L"[P]               : パストレーシングモード\n";
		t += L"[<][>]            : たいようのいどう\n";
		t += L"[ESC]             : ゲームしゅうりょう\n";

		text->SetText(t);
	}

}

void GameScene::Update() {

	if (SxavengerSystem::IsTriggerKey(KeyId::KEY_P)) {
		auto& config = FMainRender::GetInstance()->GetConfig();
		config.option.Inverse(FBaseRenderPass::Config::Option::IndirectLighting);
	}

	if (SxavengerSystem::IsPressKey(KeyId::KEY_LEFT)) {
		sunAngle_.x -= 0.01f;
	}

	if (SxavengerSystem::IsPressKey(KeyId::KEY_RIGHT)) {
		sunAngle_.x += 0.01f;
	}

	performance_->Update();

	player_->Update();
	camera_->Update();

	items_->Update();

	skyDailyCycle_->Update();
	skyDailyCycle_->GetComponent<TransformComponent>()->rotate = Quaternion::ToQuaternion(sunAngle_);

	if (items_->IsCollected()) {
		Transition transition = {};
		transition.names = { "Clear" };

		BaseScene::SetTransition(transition);
	}

}

void GameScene::SetCollisionCallback() {

	sCollisionManager->SetOnCollisionFunctionStay(
		"Player", "Wall",
		[](MAYBE_UNUSED ColliderComponent* const player, MAYBE_UNUSED ColliderComponent* const wall) {
		CollisionHelper::PushBackAABB(player, wall);
	});

	sCollisionManager->SetOnCollisionFunctionStay(
		"item", "Player",
		[](MAYBE_UNUSED ColliderComponent* const item, MAYBE_UNUSED ColliderComponent* const player) {
		// 取得状態へ遷移.
		item->GetBehaviour()->SetActive(false);
	});

}
