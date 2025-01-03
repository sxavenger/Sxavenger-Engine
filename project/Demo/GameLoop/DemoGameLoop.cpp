#include "DemoGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Performance/TimePoint.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DemoGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DemoGameLoop::Init(GameLoop::Context* context) {
	context->SetState(GameLoop::State::Init,   std::nullopt, [this]() { InitGame(); });
	context->SetState(GameLoop::State::Term,   std::nullopt, [this]() { TermGame(); });
	context->SetState(GameLoop::State::Update, std::nullopt, [this]() { UpdateGame(); });
	context->SetState(GameLoop::State::Draw,   std::nullopt, [this]() { DrawGame(); });
}

void DemoGameLoop::Term() {
}

void DemoGameLoop::InitGame() {
	demo_ = std::make_unique<DemoModelRender>();
	//demo_->Init("asset/model/demo/teapot.obj", true);
	demo_->Init("asset/model/bricks/bricks.gltf", false);
	demo_->SetToConsole();

	human_ = std::make_unique<DemoHuman>();
	human_->Init();
	human_->SetToConsole();
}

void DemoGameLoop::TermGame() {
	demo_.reset();
	human_.reset();
}

void DemoGameLoop::UpdateGame() {
	human_->Update();
}

void DemoGameLoop::DrawGame() {
}
