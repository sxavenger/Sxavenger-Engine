#include "ExampleGameLoop.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/System.h>
#include <Engine/Module/Scene/SceneFactory.h>
#include <Engine/Components/Component/ComponentHelper.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Editors/EditorEngine.h>
#include <Engine/Editors/Editor/DevelopEditor.h>

//* demo scene
#include <Demo/Scene/TitleScene.h>
#include <Demo/Scene/GameScene.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ExampleGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ExampleGameLoop::Init(Execution::Context* context) {
	context->SetProcess(Execution::Process::Init, std::nullopt, [this]() { InitSystem(); });
	context->SetProcess(Execution::Process::Term, std::nullopt, [this]() { TermSystem(); });
	context->SetProcess(Execution::Process::Update, std::nullopt, [this]() {
		if (auto develop = sEditorEngine->GetEditor<DevelopEditor>()) {
			if (!develop->IsProcessRequired()) {
				return;
			}
		}

		UpdateSystem();
	});
	context->SetProcess(Execution::Process::Render, std::nullopt, [this]() { RenderSystem(); });

	context->SetCondition([]() { return !System::ProcessMessage(); });             //!< windowが閉じられた場合
	context->SetCondition([]() { return System::IsPressKey(KeyId::KEY_ESCAPE); }); //!< ESCキーが押された
}

void ExampleGameLoop::Term() {
}

void ExampleGameLoop::InitSystem() {

	main_ = System::CreateMainWindow(
		Configuration::GetConfig().resolution,
		L"[Sxavenger Engine / Sxx Engine] Example Window"
	);
	main_->SetIcon(kPackagesDirectory / "icon" / "SxavengerEngineIcon.ico", { 32, 32 });

	// シーンをまたいで常駐するパフォーマンス表示(HUD).
	performance_ = std::make_unique<PerformanceActor>();
	performance_->SetPosition({ 1190.0f, 0.0f });

	// SceneControllerの初期化.
	// SceneFactoryへ各シーン(state)を名前で登録し, 初期stateとしてTitleSceneを積む.
	// 以降のシーン遷移は各シーンがSetTransition()で要求し, TransitionScene()で処理される.
	auto factory = std::make_unique<SceneFactory>();
	factory->Register<Demo::TitleScene>(Demo::TitleScene::kSceneName);
	factory->Register<Demo::GameScene>(Demo::GameScene::kSceneName);

	sceneController_ = std::make_unique<SceneController>();
	sceneController_->Init(std::move(factory));
	sceneController_->BeginState({ Demo::TitleScene::kSceneName });
}

void ExampleGameLoop::TermSystem() {
}

void ExampleGameLoop::UpdateSystem() {

	//-----------------------------------------------------------------------------------------
	// Scene Update
	//-----------------------------------------------------------------------------------------

	// 現在シーンが要求したシーン遷移を処理する (Update前に行い, 遷移直後のシーンから更新する).
	sceneController_->TransitionScene();

	// 現在シーンのゲームロジックを更新する.
	sceneController_->UpdateScene();

	performance_->Update();

	//-----------------------------------------------------------------------------------------
	// SystemUpdate
	//-----------------------------------------------------------------------------------------

	ComponentHelper::UpdateTransform();
	ComponentHelper::UpdateCollider();

	//-----------------------------------------------------------------------------------------
	// LateUpdate
	//-----------------------------------------------------------------------------------------

	// 現在シーンの遅延更新(大気のGPU更新など). Transform確定後・描画前に行う.
	sceneController_->LateUpdateScene();

	//-----------------------------------------------------------------------------------------
	// final Update...?
	//-----------------------------------------------------------------------------------------

	ComponentHelper::UpdateSkinning();
	ComponentHelper::UpdateAudio3d();
}

void ExampleGameLoop::RenderSystem() {

	FMainRender::GetInstance()->Render(System::GetDirectQueueContext(), main_.get());

	main_->BeginRenderWindow(System::GetDirectQueueContext());
	main_->ClearWindow(System::GetDirectQueueContext());

	FMainRender::GetInstance()->Present(System::GetDirectQueueContext(), main_.get());
	System::RenderImGui(main_->GetClient());

	main_->EndRenderWindow(System::GetDirectQueueContext());

}
