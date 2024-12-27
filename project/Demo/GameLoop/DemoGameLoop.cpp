#include "DemoGameLoop.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DemoGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DemoGameLoop::Init(GameLoop::Context* context) {
	context->SetState(GameLoop::State::Init,   std::nullopt, [this]() { InitGame(); });
	context->SetState(GameLoop::State::Term,   std::nullopt, [this]() { TermGame(); });
	context->SetState(GameLoop::State::Update, std::nullopt, [this]() { UpdateGame(); });
	context->SetState(GameLoop::State::Render, std::nullopt, [this]() { RenderGame(); });
}

void DemoGameLoop::Term() {
}

void DemoGameLoop::InitGame() {
	texture_ = SxavengerAsset::Import<Texture>("asset/textures/tile_black.png");

	model_ = SxavengerAsset::TryImport<Model>("asset/model/demo/cube.obj");
	model_.Lock()->Load(SxavengerSystem::GetMainThreadContext());
}

void DemoGameLoop::TermGame() {
}

void DemoGameLoop::UpdateGame() {
}

void DemoGameLoop::RenderGame() {
}
