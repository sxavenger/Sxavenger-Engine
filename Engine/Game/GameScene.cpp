#include "GameScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// sxavenger engine
#include <Engine/System/Sxavenger.h>
//#include <SxavengerGraphics.h>
#include <Lib/Environment.h>
//#include <ColliderManager.h>
//#include <MathLib.h>




////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Run() {

	//=========================================================================================
	// 初期化処理
	//=========================================================================================

	console->Init();
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

		console->Update();

		if (console->IsUpdateRequired()) {
			Update();
		}

		//=========================================================================================
		// 描画処理
		//=========================================================================================

		Sxavenger::TranstionAllocator();

		Draw();

		Sxavenger::EndImGuiAndDrawCall();
		Sxavenger::EndFrame();
	}

	//=========================================================================================
	// 終了処理
	//=========================================================================================

	Term();
	console->Term();

}

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class private methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Init() {

	gameCamera_ = std::make_unique<Camera3D>();
	gameCamera_->SetThisAttribute("GameCamera");
	gameCamera_->SetProjection(0.45f, static_cast<float>(kWindowSize.x) / static_cast<float>(kWindowSize.y), 0.01f, 16.0f);
	gameCamera_->SetTransform(kUnit3, kOrigin3, {0.0f, 0.0f, -4.0f});

	//SxavengerGraphics::camera3D = gameCamera_.get();
}

void GameScene::Term() {
}

void GameScene::Update() {
}

void GameScene::Draw() {

	/*demo_->Draw();*/

	{

		//* debugScreen *//
		Sxavenger::BeginOffscreen(console->GetSceneTexture());

		Sxavenger::EndOffscreen(console->GetSceneTexture());
		Sxavenger::TranstionAllocator();

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
