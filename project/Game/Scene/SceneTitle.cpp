#include "SceneTitle.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneGame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneTitle::Init() {

	component_.Init();

	title_ = std::make_unique<LSprite>();
	title_->Init();
	title_->SetTexture(SxavengerAsset::TryImport<AssetTexture>("asset/textures/title.png"));
	title_->SetSize(kMainWindowSize);

	component_.canvas_->AddLayer(title_.get());
}

void SceneTitle::Update() {

	const GamepadInput* gamepad = SxavengerSystem::GetInput()->GetGamepadInput(0);
	const KeyboardInput* keyboard = SxavengerSystem::GetInput()->GetKeyboardInput();

	if (gamepad->IsTrigger(GamepadButtonId::BUTTON_A) || keyboard->IsTrigger(KeyId::KEY_SPACE)) {
		BaseScene::RequestNextScene("Game");
	}
}

void SceneTitle::Draw() {

}

void SceneTitle::Term() {
}
