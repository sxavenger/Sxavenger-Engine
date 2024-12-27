#include "DemoGameLoop.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DemoGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DemoGameLoop::Init(GameLoop::Context* context) {
	context->SetState(GameLoop::State::Init,   std::nullopt, [this]() { InitGame(); });
	context->SetState(GameLoop::State::Term,   std::nullopt, [this]() { TermGame(); });
	context->SetState(GameLoop::State::Update, std::nullopt, [this]() { UpdateGame(); });
	context->SetState(GameLoop::State::Draw,   std::nullopt, [this]() { RenderGame(); });
}

void DemoGameLoop::Term() {
}

void DemoGameLoop::InitGame() {
	demo_ = std::make_unique<DemoModelRender>();
	demo_->Init("asset/model/demo/teapot.obj", true);
	demo_->SetToConsole();
}

void DemoGameLoop::TermGame() {
	demo_.reset();
}

void DemoGameLoop::UpdateGame() {
}

void DemoGameLoop::RenderGame() {
}
