//[DemoGameLoop.cpp]
#include "DemoGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/EngineDeveloperEditor.h>

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
}

void DemoGameLoop::TermGame() {
}

void DemoGameLoop::UpdateGame() {
}

void DemoGameLoop::DrawGame() {
	main_->BeginRendering();
	main_->ClearWindow();

	//!< ここにwindow描画処理を記述する
	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
