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

	player_ = std::make_unique<Player>();
	player_->Load();
	player_->Awake();
	player_->Start();

	light_ = ComponentHelper::CreateDirectionalLightMonoBehaviour();
	light_->GetComponent<TransformComponent>()->rotate *= Quaternion::ToQuaternion({ kPi / 2.1f, 0.0f, 0.0f });
	{
		auto& param = light_->GetComponent<DirectionalLightComponent>()->GetParameter();
		param.intensity = 3.0f;

		auto& shadow = light_->GetComponent<DirectionalLightComponent>()->GetShadowParameter();
		shadow.strength = 0.85f;
	}

	lightA_ = ComponentHelper::CreatePointLightMonoBehaviour();

	//sSceneObjects->InputJson();

	SetCollisionCallback();
}

void DemoGameLoop::TermGame() {
}

void DemoGameLoop::UpdateGame() {

	//-----------------------------------------------------------------------------------------
	// GameLogic Update
	//-----------------------------------------------------------------------------------------

	player_->Update();

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
}
