#include "SceneTitle.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Runtime/Scene/SceneController.h>
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneGame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneTitle::Init() {
	title_ = std::make_unique<LSprite>();
	title_->Init();
	title_->SetTexture(SxavengerAsset::TryImport<AssetTexture>());
}

void SceneTitle::Update() {

	const GamepadInput* gamepad = SxavengerSystem::GetInput()->GetGamepadInput(0);

	if (gamepad->IsTrigger(GamepadButtonId::BUTTON_A)) {
		controller_->RequestNextScene("Game");
	}
}

void SceneTitle::Draw() {
}

void SceneTitle::Term() {
}
