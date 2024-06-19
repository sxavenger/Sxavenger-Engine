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

	// TLAS 
	tlas_ = std::make_unique<TopLevelAS>();

	// constantBuffer
	camera_ = std::make_unique<RayTracingCamera>();
	light_  = std::make_unique<RayTracingLight>();

	// Game
	ground_ = std::make_unique<Ground>();
	player_ = std::make_unique<Player>();
	teapot_ = std::make_unique<Teapot>();

	subobjectManager_ = std::make_unique<SubobjectManager>();

	cube_ = std::make_unique<Cube>();
	cube_->Init(subobjectManager_.get());

	// drawer
	fullscreen_ = std::make_unique<FullScreen>();

	// cs
	cs_ = std::make_unique<DxObject::CSBlob>();
	cs_->Init(L"test.CS.hlsl");


}

void GameScene::Term() {
}

void GameScene::Update() {


	player_->Update();
	camera_->Update(player_->GetWorldMatrix());
	subobjectManager_->Update();

	// TLASへの書き込み
	tlas_->StartBlasSetup();

	tlas_->SetBLAS(ground_->GetBlas(), ground_->GetWorldMatrix(), 0);
	tlas_->SetBLAS(player_->GetBlas(), player_->GetWorldMatrix(), 0);
	tlas_->SetBLAS(teapot_->GetBlas(), teapot_->GetWorldMatrix(), 0);
	tlas_->SetBLAS(cube_->GetBlas(), cube_->GetWorldMatrix(), 0);

	subobjectManager_->SetBlases(tlas_.get());

	tlas_->EndBlasSetup();
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

		fullscreen_->DrawTexture(RayTracingEngine::GetDxrCommon()->GetResultBufferTexture());

		/*
			ImGuiの関係上、スクリーン描画は最後にする
		*/
	}
}
