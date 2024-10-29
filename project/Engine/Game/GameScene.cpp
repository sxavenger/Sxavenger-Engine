#include "GameScene.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// sxavenger engine
#include <Engine/System/Sxavenger.h>
#include <Engine/Game/SxavengerGame.h>
#include <Engine/Console/SystemConsole.h>
#include <Lib/Environment.h>

#include "Lib/Adapter/Random/Random.h"

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
	

	teapot_ = std::make_unique<Teapot>();
	teapot_->Init();
	teapot_->SetToConsole();

	plane_ = std::make_unique<Plane>();
	plane_->Init();
	plane_->SetToConsole();

	atmosphericScattering_ = std::make_unique<AtmosphericScattering>();
	atmosphericScattering_->Init();
	atmosphericScattering_->SetToConsole();

	particleCollection_ = std::make_unique<ParitcleCollection>();
	particleCollection_->Init();
	particleCollection_->SetToConsole();
}

void GameScene::Term() {
}

void GameScene::Update() {
	particleCollection_->Update();
}

void GameScene::Draw() {

	{
		//* main screen *//
		/*Sxavenger::BeginOffscreen(sBetaConsole->GetGameRenderTarget());

		SxavengerGame::DrawToScene(sBetaConsole->GetGameCamera());
		Sxavenger::EndOffscreen(sBetaConsole->GetGameRenderTarget());
		Sxavenger::TranstionAllocator();*/
	}

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
