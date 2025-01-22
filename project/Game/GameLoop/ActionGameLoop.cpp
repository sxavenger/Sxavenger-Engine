#include "ActionGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/!Render/FRenderCore.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/EngineDeveloperEditor.h>
#include <Engine/Editor/Editors/RenderSceneEditor.h>

//* other scene
#include "../Scene/SceneGame.h"
#include "../Scene/SceneTitle.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ActionGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////

void ActionGameLoop::Init(GameLoop::Context* context) {
	context->SetState(GameLoop::State::Init, std::nullopt, [this]() { InitGame(); });
	context->SetState(GameLoop::State::Term, std::nullopt, [this]() { TermGame(); });

	context->SetState(GameLoop::State::Update, std::nullopt, [this]() {

		if (auto developer = sEditorEngine->TryGetEditor<EngineDeveloperEditor>()) {
			if (!developer->IsProcessRequired()) {
				return;
			}
		}

		UpdateGame();
	});

	context->SetState(GameLoop::State::Draw, std::nullopt, [this]() { DrawGame(); });

	context->SetCondition(
		[this]() { return !SxavengerSystem::ProcessMessage(); }
	);
}

void ActionGameLoop::Term() {
}

void ActionGameLoop::InitGame() {

	FRenderCore::GetInstance()->Init();

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, kMainWindowTitle).lock();

	std::unique_ptr<BaseSceneFactory> factory = std::make_unique<BaseSceneFactory>();
	factory->Register<SceneGame>("Game");
	factory->Register<SceneTitle>("Title");

	collection_ = std::make_unique<SceneController>();
	collection_->SetSceneFactory(std::move(factory));
	collection_->Init("Title");

	textures_ = std::make_unique<FSceneTextures>();
	textures_->Create(main_->GetSize());

	presenter_ = std::make_unique<FPresenter>();
	presenter_->Init();
}

void ActionGameLoop::TermGame() {
	collection_.reset();
}

void ActionGameLoop::UpdateGame() {
	collection_->ActivateNextScene();
	collection_->UpdateScene();

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([this](RenderSceneEditor* editor) {
		editor->SetGameRenderer(collection_->GetScene()->GetComponent().renderer_.get());
	});
}

void ActionGameLoop::DrawGame() {
	collection_->DrawScene();
	collection_->GetScene()->GetComponent().Render(SxavengerSystem::GetMainThreadContext(), textures_.get());

	main_->BeginRendering();
	main_->ClearWindow();

	presenter_->Present(SxavengerSystem::GetMainThreadContext(), main_->GetSize(), textures_->GetGBuffer(FSceneTextures::GBufferLayout::Result)->GetGPUHandleSRV());

	SxavengerSystem::RenderImGui();

	main_->EndRendering();

}
