#include "GameScene.h"

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

	sBetaConsole->Init();
	//sSystemConsole->Init();
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

		sBetaConsole->Update();
		//sSystemConsole->UpdateConsole();

		if (sBetaConsole->IsUpdateRequired()) {
			Update();
		}

		//=========================================================================================
		// 描画処理
		//=========================================================================================

		Sxavenger::TranstionAllocator();

		sBetaConsole->Draw();
		Draw();

		Sxavenger::EndImGuiAndDrawCall();
		Sxavenger::EndFrame();

		SxavengerGame::ResetPrimitive();
	}

	//=========================================================================================
	// 終了処理
	//=========================================================================================

	Term();
	sBetaConsole->Term();

}

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class private methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Init() {
	

	rail_ = std::make_unique<Rail>();
	rail_->Init();
	rail_->SetToConsole();

	player_ = std::make_unique<Player>();
	player_->Init();
	player_->SetToConsole();
	player_->SetRail(rail_.get());

}

void GameScene::Term() {
}

void GameScene::Update() {
	player_->Update();
}

void GameScene::Draw() {

	{
		//* main screen *//
		Sxavenger::BeginOffscreen(sBetaConsole->GetGameRenderTarget());

		SxavengerGame::DrawToScene(sBetaConsole->GetGameCamera());
		Sxavenger::EndOffscreen(sBetaConsole->GetGameRenderTarget());
		Sxavenger::TranstionAllocator();
	}

	//=========================================================================================
	// スクリーン描画処理
	//=========================================================================================

	{
		Sxavenger::BeginScreenDraw();
		
		// "offscreen"をフルスクリーンにする
		Sxavenger::GetDxCommon()->CopyResource(
			Sxavenger::GetDxCommon()->GetSwapChainObj()->GetBackBufferResource(), D3D12_RESOURCE_STATE_RENDER_TARGET,
			sBetaConsole->GetGameRenderTarget()->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

		
	}
}
