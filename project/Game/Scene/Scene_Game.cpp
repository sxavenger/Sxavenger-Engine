#include "Scene_Game.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>

//* othre scene
#include <Game/Scene/Scene_Title.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Scene_Game class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Init() {
	SxavengerGame::ClearColliders();
	sSystemConsole->SetRaytracingEnable(true);

	SxavengerGame::LoadModel("resources/model/CG2", "axis.obj");

	player_ = std::make_unique<Player>();
	player_->Init();
	player_->SetToConsole();

	enemyCollection_ = std::make_unique<EnemyCollection>();
	enemyCollection_->Init();
	enemyCollection_->SetToConsole();
	enemyCollection_->SetPlayer(player_.get());

	field_ = std::make_unique<Field>();
	field_->Init();
	field_->SetToConsole();

	particle_ = std::make_unique<ParticleCollection>();
	particle_->Init();
	particle_->SetToConsole();

	enemyCollection_->SetParticleCollection(particle_.get());
}

void Scene_Game::Term() {
}

void Scene_Game::Update() {

	player_->Update();
	enemyCollection_->Update();

	if (player_->IsEnd()) {
		SetNextScene<Scene_Title>();
	}

	particle_->Update();

	SxavengerGame::UpdateColliders();

}

void Scene_Game::Draw() {
}
