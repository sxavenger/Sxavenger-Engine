#include "SceneManager.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SceneManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneManager::Init(std::unique_ptr<IScene>& scene) {
	scene_ = std::move(scene);
	scene_->Init();
}

void SceneManager::Update() {
	scene_->Update();
}

void SceneManager::Draw() {
	scene_->Draw();
}

void SceneManager::ChangeScene() {
	if (scene_->IsRequsetedNextScene()) {
		scene_ = scene_->GetNextScene(); //!< シーンの変更
		scene_->Init();                  //!< シーンの初期化
	}
}