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

	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->Init();

}

void GameScene::Term() {
}

void GameScene::Update() {
	railCamera_->Update();
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
