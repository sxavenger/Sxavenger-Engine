#include "Scene_Title.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* other scene
#include "Scene_Game.h"

//* engine
#include <Engine/Game/SxavengerGame.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Scene_Title class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Scene_Title::Init() {
	sSystemConsole->SetRaytracingEnable(false);

	Sxavenger::LoadTexture("resources/textures/title.png");
}

void Scene_Title::Term() {
}

void Scene_Title::Update() {
	if (Sxavenger::IsTriggerKey(DIK_SPACE)) {
		SetNextScene<Scene_Game>();
	}
}

void Scene_Title::Draw() {

	sSystemConsole->GetGameFrame()->BeginAdaptive();

	SxavengerGame::DrawSprite(
		{ 0.0f, 0.0f }, kWindowSize, Sxavenger::GetTextureHandleGPU("resources/textures/title.png")
	);

	sSystemConsole->GetGameFrame()->EndAdaptive();

}
