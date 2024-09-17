#include "GameScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// sxavenger engine
#include <Engine/System/Sxavenger.h>
//#include <SxavengerGraphics.h>
#include <Lib/Environment.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Run() {

	//=========================================================================================
	// 初期化処理
	//=========================================================================================

	sBetaConsole->Init();
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

	teapot_ = std::make_unique<Teapot>();
	teapot_->Init();

}

void GameScene::Term() {
}

void GameScene::Update() {
}

void GameScene::Draw() {

	{

		//* main screen *//
		Sxavenger::BeginOffscreen(Sxavenger::GetTexture<RenderTexture>("main"));

		Sxavenger::EndOffscreen(Sxavenger::GetTexture<RenderTexture>("main"));
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
			Sxavenger::GetTexture<RenderTexture>("main")->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

		
	}
}