#include "SceneClear.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneGame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneClear::Init() {
	component_.Init();

	clear_ = std::make_unique<LSprite>();
	clear_->Init();
	clear_->SetTexture(SxavengerAsset::TryImport<AssetTexture>("asset/textures/clear.png"));
	clear_->SetSize(kMainWindowSize);

	component_.canvas_->AddLayer(clear_.get());
}

void SceneClear::Update() {
	const GamepadInput* gamepad   = SxavengerSystem::GetInput()->GetGamepadInput(0);
	const KeyboardInput* keyboard = SxavengerSystem::GetInput()->GetKeyboardInput();

	if (gamepad->IsTrigger(GamepadButtonId::BUTTON_A) || keyboard->IsTrigger(KeyId::KEY_SPACE)) {
		BaseScene::RequestNextScene("Title");
	}
}

void SceneClear::Draw() {
}

void SceneClear::Term() {
}
