#include "TitleScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TitleScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TitleScene::Init() {

}

void TitleScene::Update() {

	const KeyboardInput* keyboard = SxavengerSystem::GetKeyboardInput();

	if (keyboard->IsTrigger(KeyId::KEY_SPACE)) {
		Transition transition = {};
		transition.destruction = Transition::Destruction::Single;

		BaseScene::SetTransition(transition);
	}

}
