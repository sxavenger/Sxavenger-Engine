#include "ConsoleGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/Console.h>
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ConsoleGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ConsoleGameLoop::Init(GameLoop::Context* context) {

	context->SetState(
		GameLoop::State::Init, std::nullopt, [this]() {
			sConsole->Init();

			main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, kMainWindowTitle).lock();
			main_->SetIcon("packages/icon/SxavengerEngineIcon.ico", { 32, 32 });

			sConsole->SetWindow(main_);
		}
	);

	context->SetState(
		GameLoop::State::Term, std::nullopt, [this]() {
			sConsole->Term();
		}
	);

	context->SetState(
		GameLoop::State::Update, std::nullopt, [this]() {
			sConsole->UpdateConsole();
		}
	);

	context->SetState(
		GameLoop::State::Render, UINT32_MAX, [this]() {
			sConsole->Draw();
			sConsole->DrawConsole();
		}
	);

	context->SetCondition([this]() {
		return !SxavengerSystem::ProcessMessage();
	});
}

void ConsoleGameLoop::Term() {
}
