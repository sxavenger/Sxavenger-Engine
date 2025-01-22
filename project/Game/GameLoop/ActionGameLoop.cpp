#include "ActionGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/SxavengerModule.h>

//* other scene
#include "../Scene/SceneGame.h"
#include "../Scene/SceneTitle.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ActionGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////

void ActionGameLoop::Init(GameLoop::Context* context) {
	context->SetState(GameLoop::State::Init, std::nullopt, [this]() { InitGame(); });
	context->SetState(GameLoop::State::Term, std::nullopt, [this]() { TermGame(); });
	context->SetState(GameLoop::State::Update, std::nullopt, [this]() { UpdateGame(); });
	context->SetState(GameLoop::State::Draw, std::nullopt, [this]() { DrawGame(); });
}

void ActionGameLoop::Term() {
}

void ActionGameLoop::InitGame() {

	std::unique_ptr<BaseSceneFactory> factory = std::make_unique<BaseSceneFactory>();
	factory->Register<SceneGame>("Game");
	factory->Register<SceneTitle>("Title");

	collection_ = std::make_unique<SceneController>();
	collection_->SetSceneFactory(std::move(factory));
	collection_->Init("Title");
}

void ActionGameLoop::TermGame() {
	collection_.reset();
}

void ActionGameLoop::UpdateGame() {
	collection_->ActivateNextScene();
	collection_->UpdateScene();
}

void ActionGameLoop::DrawGame() {
	collection_->DrawScene();
}
