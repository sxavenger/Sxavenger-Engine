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

	component_.Init();

	camera_ = std::make_unique<GameCamera>();
	camera_->Init();
	component_.renderer_->SetCamera(camera_.get());

	player_ = std::make_unique<Player>();

	QuaternionTransform transform = {};
	transform.translate = { 0.0f, 0.0f, -8.0f };
	transform.rotate = Quaternion::Identity();
	player_->SetCamera(camera_.get());
	player_->Init(transform);
	player_->SetToOutliner();
	component_.scene_->AddGeometry(player_.get());
	

	enemy_ = std::make_unique<Enemy>();
	enemy_->Init();
	enemy_->SetToOutliner();
	component_.scene_->AddGeometry(enemy_.get());

	ground_ = std::make_unique<Ground>();
	ground_->Init();
	component_.scene_->AddGeometry(ground_.get());

	light_ = std::make_unique<ADirectionalLightActor>();
	light_->Init();
	light_->GetTransform().rotate = MakeAxisAngle({1.0f, 0.0f, 0.0f}, -pi_v / 2.0f + 0.01f);
	light_->UpdateMatrix();

	component_.scene_->AddLight(light_.get());

	ambient_ = std::make_unique<FAmbientProcessSetting>();
	component_.renderer_->SetAmbientSetting(ambient_.get());

	atmosphere_ = std::make_unique<FAmbientProcessAtmosphere>();
	atmosphere_->Init();
	ambient_->AddProcess(atmosphere_.get());

}

void SceneGame::Update() {

	camera_->Update();
	player_->Update();
	enemy_->Update();

	SxavengerModule::CheckCollision();

	if (enemy_->IsDead()) {
		BaseScene::RequestNextScene("Title");
	}
}

void SceneGame::Draw() {
}

void SceneGame::Term() {
}
