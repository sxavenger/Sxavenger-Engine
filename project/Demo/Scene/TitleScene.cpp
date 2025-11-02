#include "TitleScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TitleScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TitleScene::Init() {

	camera_ = std::make_unique<MonoBehaviour>();
	camera_->SetName("title_camera");
	camera_->LoadComponent(kAssetsDirectory / "behaviour" / "title_camera.behaviour");

}

void TitleScene::Start() {

	title_ = std::make_unique<MonoBehaviour>();
	title_->SetName("title");
	title_->LoadComponent(kAssetsDirectory / "behaviour" / "title.behaviour");
	
}

void TitleScene::Update() {

	const KeyboardInput* keyboard = SxavengerSystem::GetKeyboardInput();

	if (keyboard->IsPressAny()) {
		Transition transition = {};
		transition.destruction = Transition::Destruction::Single;

		BaseScene::SetTransition(transition);
	}

}
