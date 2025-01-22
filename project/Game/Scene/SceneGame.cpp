#include "SceneGame.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/SxavengerModule.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneGame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneGame::Init() {
	/*player_ = std::make_unique<Player>();

	QuaternionTransform transform = {};
	transform.translate = { 0.0f, 0.0f, -8.0f };
	transform.rotate = Quaternion::Identity();
	player_->Init(transform);
	player_->SetToConsole();

	enemy_ = std::make_unique<Enemy>();
	enemy_->Init();
	enemy_->SetToConsole();

	ground_ = std::make_unique<Ground>();
	ground_->Init();
	ground_->SetToConsole();*/
}

void SceneGame::Update() {
	//player_->Update();
	//enemy_->Update();

	//SxavengerModule::CheckCollision();

	//if (enemy_->IsDead()) {
	//	controller_->RequestNextScene("Title");
	//}
}

void SceneGame::Draw() {
}

void SceneGame::Term() {
}
