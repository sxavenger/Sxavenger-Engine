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

#include <DxShaderReflection.h>

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

	/*auto blob = std::make_unique<DxObject::GraphicsBlob>();
	blob->Create(L"GaussianBlur/GaussianBlur.PS.hlsl", DxObject::GRAPHICS_PIXEL);

	auto reflection = std::make_unique<DxObject::ShaderReflection>();
	reflection->Init(blob->GetGraphicsBlobs()[DxObject::GRAPHICS_PIXEL]);*/

	light_ = std::make_unique<Light>(MyEngine::GetDevicesObj());

	cube_ = std::make_unique<AnimationCube>();
	human_ = std::make_unique<AnimationHuman>();
	skybox_ = std::make_unique<Skybox>();

}

void GameScene::Term() {
}

void GameScene::Update() {
	cube_->Update();
	human_->Update();
}

void GameScene::Draw() {

	{
		//* debugScreen *//
		MyEngine::BeginOffscreen(console->GetSceneTexture());
		MyEngine::camera3D_ = console->GetDebugCamera();

		MyEngine::EndOffscreen(console->GetSceneTexture());
		MyEngine::TransitionProcess();

		//* main screen *//
		MyEngine::BeginOffscreen(MyEngine::GetTexture("offscreen"));

		cube_->Draw();
		human_->Draw();

		MyEngine::EndOffscreen(MyEngine::GetTexture("offscreen"));
	}

	MyEngine::TransitionProcess();

	//=========================================================================================
	// スクリーン描画処理
	//=========================================================================================

	{
		MyEngine::BeginScreenDraw();

		MyEngine::GetDxCommon()->CopyResource(
			MyEngine::GetDxCommon()->GetSwapChainObj()->GetResource(MyEngine::GetDxCommon()->GetSwapChainObj()->GetCurrentBackBufferIndex()), D3D12_RESOURCE_STATE_RENDER_TARGET,
			MyEngine::GetTexture("offscreen")->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

		/*
			ImGuiの関係上、スクリーン描画は最後にする
		*/
	}
}
