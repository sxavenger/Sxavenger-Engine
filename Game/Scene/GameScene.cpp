#include "GameScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// sxavenger engine
#include <MyEngine.h>
#include <DirectXRCommon.h>
#include <Environment.h>

#include "ColliderManager.h"

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

		if (console->IsUpdateRequired()) {
			Update();
		}

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

	gameCamera_ = std::make_unique<Camera3D>();
	gameCamera_->SetThisAttribute("GameCamera");
	gameCamera_->SetProjection(0.45f, static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight), 0.01f, 16.0f);
	gameCamera_->SetTransform(unitVector, origin, {0.0f, 0.0f, -4.0f});

	MyEngine::camera3D = gameCamera_.get();

	debugObjectManager_ = std::make_unique<DebugObjectManager>();
	debugObjectManager_->Init();

	particle_ = std::make_unique<Particle>();
	particle_->Init();

	/*{ //!< test
		std::unique_ptr<DxObject::GraphicsBlob> blob = std::make_unique<DxObject::GraphicsBlob>();
		blob->Create(L"object3d/object3d.vs.hlsl", DxObject::GRAPHICS_VERTEX);
		blob->Create(L"object3d/object3d.ps.hlsl", DxObject::GRAPHICS_PIXEL);

		std::unique_ptr<DxObject::ShaderReflectionTable> reflection1
			= std::make_unique<DxObject::ShaderReflectionTable>();

		std::unique_ptr<DxObject::ShaderReflectionTable> reflection2
			= std::make_unique<DxObject::ShaderReflectionTable>();

		reflection1->Create(blob->GetGraphicsBlobs()[DxObject::GRAPHICS_PIXEL], DxObject::VISIBILITY_PIXEL);
		reflection2->Create(blob->GetGraphicsBlobs()[DxObject::GRAPHICS_VERTEX], DxObject::VISIBILITY_VERTEX);

		reflection1->Marge(reflection2.get());

		reflection1->Bind("gTexture", MyEngine::GetTextureHandleGPU("resources/uvChecker.png"));
	}*/

}

void GameScene::Term() {
}

void GameScene::Update() {

	debugObjectManager_->Update();

	particle_->Update();

	ColliderManager::GetInstance()->Update();
}

void GameScene::Draw() {

	{
		//* debugScreen *//
		MyEngine::BeginOffscreen(console->GetSceneTexture());
		MyEngine::camera3D = console->GetDebugCamera();

		debugObjectManager_->Draw();
		particle_->Draw();

		ColliderManager::GetInstance()->DrawColliders();

		MyEngine::EndOffscreen(console->GetSceneTexture());
		MyEngine::TransitionProcess();

		//* main screen *//
		MyEngine::BeginOffscreen(MyEngine::GetTexture("offscreen"));
		MyEngine::camera3D = gameCamera_.get();

		debugObjectManager_->Draw();

		MyEngine::EndOffscreen(MyEngine::GetTexture("offscreen"));
		MyEngine::TransitionProcess();
	}

	//=========================================================================================
	// スクリーン描画処理
	//=========================================================================================

	{
		MyEngine::BeginScreenDraw();
		
		// "offscreen"をフルスクリーンにする
		MyEngine::GetDxCommon()->CopyResource(
			MyEngine::GetDxCommon()->GetSwapChainObj()->GetBackBufferResource(), D3D12_RESOURCE_STATE_RENDER_TARGET,
			MyEngine::GetTexture("offscreen")->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);

		/*
			ImGuiの関係上、スクリーン描画は最後にする
		*/
	}
}
