#include "GameScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// sxavenger engine
#include <Sxavenger.h>
#include <SxavengerGraphics.h>
#include <Environment.h>
#include <ColliderManager.h>
#include <MathLib.h>


////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Run() {

	//=========================================================================================
	// 初期化処理
	//=========================================================================================

	console->Init();
	Init();

	Sxavenger::TransitionProcessSingle();

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

		Sxavenger::TransitionProcess();

		Draw();

		SxavengerGraphics::ResetPrimitives();
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
	gameCamera_->SetProjection(0.45f, static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight), 0.01f, 16.0f);
	gameCamera_->SetTransform(unitVector, origin, {0.0f, 0.0f, -4.0f});

	SxavengerGraphics::camera3D = gameCamera_.get();

	debugObjectManager_ = std::make_unique<DebugObjectManager>();
	debugObjectManager_->Init();

	particle_ = std::make_unique<Particle>();
	particle_->Init();

	/*demo_ = std::make_unique<RayTracingDemo>();
	demo_->Init();*/

	objectDemo_ = std::make_unique<ObjectDemo>();
	objectDemo_->Init();
	
}

void GameScene::Term() {
}

void GameScene::Update() {

	/*demo_->Update();*/

	debugObjectManager_->Update();

	particle_->Update();

	ColliderManager::GetInstance()->Update();
}

void GameScene::Draw() {

	/*demo_->Draw();*/

	{

		//* debugScreen *//
		Sxavenger::BeginOffscreen(console->GetSceneTexture());
		SxavengerGraphics::camera3D = console->GetDebugCamera();

		debugObjectManager_->Draw();
		objectDemo_->Draw();

		ColliderManager::GetInstance()->DrawColliders();

		for (int i = 0; i < 100; ++i) {
			float theta = pi_v * 2.0f / 100 * i;

			SxavengerGraphics::DrawLine(
				{ std::sin(theta), 0.0f, std::cos(theta)  },
				{ -std::sin(theta), 0.0f, -std::cos(theta) },
				ToColor4f(0xFAFA00FF)
			);
		}

		SxavengerGraphics::DrawTriangle(
			{ 1.0f, 1.0f, 1.0f },
			{ -1.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f },
			ToColor4f(0xFAFAFAFF)
		);

		SxavengerGraphics::DrawAll3D();

		Sxavenger::EndOffscreen(console->GetSceneTexture());
		Sxavenger::TransitionProcess();

		//* main screen *//
		Sxavenger::BeginOffscreen(Sxavenger::GetTexture<RenderTexture>("offscreen"));
		SxavengerGraphics::camera3D = gameCamera_.get();

		debugObjectManager_->Draw();
		objectDemo_->Draw();

		for (int i = 0; i < 20; ++i) {
			float theta = pi_v * 2.0f / 20 * i;

			SxavengerGraphics::DrawLine(
				{ std::sin(theta),  std::cos(theta), 0.0f },
				{ -std::sin(theta), -std::cos(theta), 0.0f },
				ToColor4f(0xFA0000FF)
			);
		}

		SxavengerGraphics::DrawAll3D();

		Sxavenger::EndOffscreen(Sxavenger::GetTexture<RenderTexture>("offscreen"));
		Sxavenger::TransitionProcess();
	}

	//=========================================================================================
	// スクリーン描画処理
	//=========================================================================================

	{
		Sxavenger::BeginScreenDraw();
		
		// "offscreen"をフルスクリーンにする
		Sxavenger::GetDxCommon()->CopyResource(
			Sxavenger::GetDxCommon()->GetSwapChainObj()->GetBackBufferResource(), D3D12_RESOURCE_STATE_RENDER_TARGET,
			Sxavenger::GetTexture<RenderTexture>("offscreen")->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

		/*
			ImGuiの関係上、スクリーン描画は最後にする
		*/
	}
}
