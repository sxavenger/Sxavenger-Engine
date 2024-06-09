#include "GameScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// sxavenger engine
#include <MyEngine.h>
#include <DirectXRCommon.h>
#include <Environment.h>

// DxrObject
#include <DxrAccelerationStructure.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxrObject;

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Run() {

	//=========================================================================================
	// 初期化処理
	//=========================================================================================

	console->Init();
	Init();

	/*std::unique_ptr<Camera3D> camera = std::make_unique<Camera3D>();
	MyEngine::camera3D_ = camera.get();*/

	MyEngine::TransitionProcessSingle();
	MyEngine::EnableTextures();

	////////////////////////////////////////////////////////////////////////////////////////////
	// メインループ
	////////////////////////////////////////////////////////////////////////////////////////////
	while (MyEngine::ProcessMessage() == 0) {

		MyEngine::BeginFrame();

		//=========================================================================================
		// 更新処理
		//=========================================================================================

		console->Update();
		Update();

		//=========================================================================================
		// 描画処理
		//=========================================================================================

		MyEngine::TransitionProcess();
		MyEngine::BeginDraw();

		Draw();

		MyEngine::EndFrame();

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

	// constantBuffer //

	camera_ = std::make_unique<RayTracingCamera>();
	light_  = std::make_unique<RayTracingLight>();

	// Game

	ground_ = std::make_unique<Ground>();
	player_ = std::make_unique<Player>();
	teapot_ = std::make_unique<Teapot>();


	// TLASにBLASの設定
	tlas_ = std::make_unique<TopLevelAS>();
	tlas_->SetBLAS(player_->GetBlas(), player_->GetWorldMatrixPtr(), 0);
	tlas_->SetBLAS(ground_->GetBlas(), ground_->GetWorldMatrixPtr(), 0);
	tlas_->Init();
}

void GameScene::Term() {
}

void GameScene::Update() {

	player_->Update();
	camera_->Update(player_->GetWorldMatrix());

	static int frame = 0;
	frame++;

	if (frame == 60 * 5) {
		//tlas_->EraseBLAS(player_->GetBlas());
		//tlas_->EraseBLAS(ground_->GetBlas());
		tlas_->SetBLAS(teapot_->GetBlas(), teapot_->GetWorldMatrixPtr(), 0);

		console->SetLog(
			"- point1 path",
			Console::commentOutColor
		);
	}

	if (frame == 60 * 10) {
		tlas_->EraseBLAS(player_->GetBlas());
		tlas_->EraseBLAS(ground_->GetBlas());

		console->SetLog(
			"- point2 path",
			Console::commentOutColor
		);
	}

	// TLASの更新
	tlas_->Update();
}

void GameScene::Draw() {

	auto commandList = MyEngine::GetCommandList();

	//=========================================================================================
	// レイトレーシング描画処理
	//=========================================================================================

	{
		RayTracingEngine::BeginRayTracing(tlas_.get());

		commandList->SetComputeRootDescriptorTable(0, tlas_->GetGPUDescriptorHandle());
		commandList->SetComputeRootConstantBufferView(1, camera_->GetGPUVirtualAddress());
		commandList->SetComputeRootConstantBufferView(2, light_->GetGPUVirtualAddress());

		RayTracingEngine::EndRayTracing();
	}

	MyEngine::TransitionProcess();
	console->OutputRayTracingResult(RayTracingEngine::GetDxrCommon()->GetResultBufferTexture());

	/*//-----------------------------------------------------------------------------------------
	// postEffect test
	//-----------------------------------------------------------------------------------------

	{
		blur->CreateBlurTexture(kWindowWidth, kWindowHeight, RayTracingEngine::GetDxrCommon()->GetResultBufferTexture());
	}
	MyEngine::TransitionProcess();
	console->OutputTexture("blur", blur->GetTexture());*/

	/*//=========================================================================================
	// オフスクリーン描画処理
	//=========================================================================================

	{
		// todo: 複数へのtexture書き込みをさせる.
		MyEngine::BeginOffScreen(console_->GetSceneTexture());
		MyEngine::camera3D_ = console_->GetDebugCamera();

		RayTracingEngine::GetDxrCommon()->DrawRasterlize();

		MyEngine::EndOffScreen();


		MyEngine::BeginOffScreen(MyEngine::GetTexture("offscreen"));
		MyEngine::camera3D_ = camera.get();



		MyEngine::EndOffScreen();
	}

	MyEngine::TransitionProcess();*/

	//=========================================================================================
	// スクリーン描画処理
	//=========================================================================================

	{
		MyEngine::BeginScreenDraw();

		/*
			ImGuiの関係上、スクリーン描画は最後にする
		*/
	}
}
