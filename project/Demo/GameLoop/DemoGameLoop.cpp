//[DemoGameLoop.cpp]
#include "DemoGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/EngineDeveloperEditor.h>
#include <Engine/Editor/Editors/RenderSceneEditor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DemoGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DemoGameLoop::Init(GameLoop::Context* context) {
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

	context->SetCondition([this]() { return !SxavengerSystem::ProcessMessage(); });
}

void DemoGameLoop::Term() {
}

void DemoGameLoop::InitGame() {
	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"Demo Window").lock();
	main_->SetIcon("packages/icon/SxavengerEngineSubIcon.ico", { 32, 32 });

	//* render関係 *//

	textures_ = std::make_unique<FSceneTextures>();
	textures_->Create(main_->GetSize());

	presenter_.Init();

	renderer_ = std::make_unique<FSceneRenderer>();
	renderer_->SetTextures(textures_.get());

	scene_ = std::make_unique<FScene>();
	renderer_->SetScene(scene_.get());

	//* editor *//

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([this](RenderSceneEditor* editor) {
		editor->SetGameRenderer(renderer_.get());
	});
}

void DemoGameLoop::TermGame() {
}

void DemoGameLoop::UpdateGame() {
}

void DemoGameLoop::DrawGame() {

	scene_->SetupTopLevelAS(SxavengerSystem::GetMainThreadContext());
	renderer_->Render(SxavengerSystem::GetMainThreadContext());

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([](RenderSceneEditor* editor) {
		editor->Render();
	});

	main_->BeginRendering();
	main_->ClearWindow();

	presenter_.Present(SxavengerSystem::GetMainThreadContext(), main_->GetSize(), textures_->GetGBuffer(FSceneTextures::GBufferLayout::Main)->GetGPUHandleSRV());

	//!< ここにwindow描画処理を記述する
	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
