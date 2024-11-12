#include "GameScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// sxavenger engine
#include <Engine/System/Sxavenger.h>
#include <Engine/Game/SxavengerGame.h>
#include <Engine/Console/SystemConsole.h>

//* othre scene
#include <Game/Scene/Scene_Title.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Run() {

	//=========================================================================================
	// 初期化処理
	//=========================================================================================

	Init();

	Sxavenger::ResetBackAllocator();

	////////////////////////////////////////////////////////////////////////////////////////////
	// メインループ
	////////////////////////////////////////////////////////////////////////////////////////////
	while (Sxavenger::ProcessMessage() == 0) {

		Sxavenger::BeginFrame();

		//=========================================================================================
		// 更新処理
		//=========================================================================================

		sSystemConsole->UpdateConsole();

		if (sSystemConsole->IsUpdateRequired()) {
			Update();
		}

		//=========================================================================================
		// 描画処理
		//=========================================================================================

		Sxavenger::TranstionAllocator();

		sSystemConsole->Draw();
		Draw();

		Sxavenger::EndImGuiAndDrawCall();
		Sxavenger::EndFrame();

		SxavengerGame::Reset();
		manager_->ChangeScene();
	}

	//=========================================================================================
	// 終了処理
	//=========================================================================================

	Term();

}

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class private methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Init() {

	atmosphericScattering_ = std::make_unique<AtmosphericScattering>();
	atmosphericScattering_->Init();
	atmosphericScattering_->SetToConsole();

	manager_ = std::make_unique<SceneManager>();
	manager_->Init<Scene_Title>();
}

void GameScene::Term() {
}

void GameScene::Update() {
	manager_->Update();
}

void GameScene::Draw() {

	manager_->Draw();

	//=========================================================================================
	// スクリーン描画処理
	//=========================================================================================

	{
		Sxavenger::BeginScreenDraw();
		
		// screenにframeを描画
		//!< screenに描画する画面は [MainMenu > Config] から設定
		sSystemConsole->PresentToScreen();
		
	}
}
