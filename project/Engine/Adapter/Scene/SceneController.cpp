#include "SceneController.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>
#include <Engine/Editor/EditorEngine.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneController class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneController::Init(const std::string& startSceneKey) {
	//* init begin scene
	if (factory_ != nullptr) {
		scene_ = factory_->CreateScene(startSceneKey);
	}

	InitScene();
}

void SceneController::Term() {
	TermScene();
	nextScene_.reset();
}

void SceneController::RequestNextScene(const std::string& key) {
	if (factory_ != nullptr) {
		nextScene_ = factory_->CreateScene(key);
	}
}

void SceneController::ActivateNextScene() {
	if (nextScene_ != nullptr) {
		TermScene();
		scene_ = std::move(nextScene_);
		InitScene();
	}
}

void SceneController::InitScene() {
	if (scene_ != nullptr) {
		scene_->Init();
		scene_->SetController(this);
	}
}

void SceneController::UpdateScene() {
	if (scene_ != nullptr) {
		scene_->Update();
	}
}

void SceneController::DrawScene() {
	if (scene_ != nullptr) {
		scene_->Draw();
	}
}

void SceneController::TermScene() {
	if (scene_ != nullptr) {
		scene_->Term();
	}
}
