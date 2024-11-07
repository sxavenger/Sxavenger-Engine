#include "Scene_Title.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* other scene
#include "Scene_Game.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Scene_Title class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Scene_Title::Init() {

	teapot_ = std::make_unique<Teapot>();
	teapot_->Init();
	teapot_->SetToConsole();

	plane_ = std::make_unique<Plane>();
	plane_->Init();
	plane_->SetToConsole();

	animationTest_ = std::make_unique<AnimationPlayer>();
	animationTest_->Init();
	animationTest_->SetToConsole();

}

void Scene_Title::Term() {
}

void Scene_Title::Update() {
	if (Sxavenger::IsTriggerKey(DIK_SPACE)) {
		SetNextScene<Scene_Game>();
	}

	animationTest_->Update();
}

void Scene_Title::Draw() {
}
