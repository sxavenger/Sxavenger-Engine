#include "DemoGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Component/ComponentHelper.h>
#include <Engine/Component/Components/Collider/CollisionManager.h>
#include <Engine/Module/Scene/SceneObjects.h>

//* demo
#include <Demo/Scene/TitleScene.h>
#include <Demo/Scene/GameScene.h>
#include <Demo/Scene/ClearScene.h>

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

	context->SetCondition([this]() { return !SxavengerSystem::ProcessMessage(); });             //!< windowが閉じられた
	context->SetCondition([this]() { return SxavengerSystem::IsPressKey(KeyId::KEY_ESCAPE); }); //!< ESCキーが押された
	context->SetCondition([this]() { return controller_->IsEmpty(); });                         //!< シーンが空になった
}

void DemoGameLoop::Term() {
}

void DemoGameLoop::InitGame() {

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"Sxavenger Engine Demo").lock();
	main_->SetIcon("packages/icon/SxavengerEngineIcon.ico", { 32, 32 });

	std::unique_ptr<SceneFactory> factory = std::make_unique<SceneFactory>();
	factory->Register<TitleScene>("Title");
	factory->Register<GameScene>("Game");
	factory->Register<ClearScene>("Clear");

	controller_ = std::make_unique<SceneController>();
	controller_->Init(std::move(factory));
	controller_->BeginState({ "Game", "Title" });

}

void DemoGameLoop::TermGame() {
	controller_.reset();
	sSceneObjects->Clear();
	// hack: ここに記述していいのか?
}

void DemoGameLoop::UpdateGame() {

	//-----------------------------------------------------------------------------------------
	// GameLogic Update
	//-----------------------------------------------------------------------------------------

	controller_->TransitionScene();
	controller_->UpdateScene();


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

	controller_->LateUpdateScene();

	//-----------------------------------------------------------------------------------------
	// FinalUpdate
	//-----------------------------------------------------------------------------------------

	ComponentHelper::UpdateSkinning();
	ComponentHelper::UpdateParticle();
	// todo: engine側のgameloopに移動.

}

void DemoGameLoop::DrawGame() {

	auto renderer = FMainRender::GetInstance();

	renderer->Render(SxavengerSystem::GetDirectQueueContext(), main_.get());

	main_->BeginRenderWindow(SxavengerSystem::GetDirectQueueContext());
	main_->ClearWindow(SxavengerSystem::GetDirectQueueContext());

	renderer->PresentMain(SxavengerSystem::GetDirectQueueContext());
	SxavengerSystem::RenderImGui();
	
	main_->EndRenderWindow(SxavengerSystem::GetDirectQueueContext());
}
