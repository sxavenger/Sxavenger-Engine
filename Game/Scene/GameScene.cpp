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

	defferedRendering_ = std::make_unique<DefferedRendering>();

	// particle
	particle_ = std::make_unique<Particle>();
	particle_->Init(defferedRendering_.get());

	nodeModel_ = std::make_unique<NodeModel>();
	nodeModel_->Init(defferedRendering_.get());

	light_ = std::make_unique<Light>(MyEngine::GetDevicesObj());

	bunny_ = std::make_unique<MeshBunny>();

	bloom_ = std::make_unique<Bloom>();


}

void GameScene::Term() {
}

void GameScene::Update() {

	particle_->Update();
	nodeModel_->Update();
}

void GameScene::Draw() {

	/*auto commandList = MyEngine::GetCommandList();*/

	//=========================================================================================
	// レイトレーシング描画処理
	//=========================================================================================

	/*{
		RayTracingEngine::BeginRayTracing(tlas_.get());

		commandList->SetComputeRootDescriptorTable(0, tlas_->GetGPUDescriptorHandle());
		commandList->SetComputeRootConstantBufferView(1, camera_->GetGPUVirtualAddress());
		commandList->SetComputeRootConstantBufferView(2, light_->GetGPUVirtualAddress());

		RayTracingEngine::EndRayTracing();
	}

	MyEngine::TransitionProcess();
	console->OutputRayTracingResult(RayTracingEngine::GetDxrCommon()->GetResultBufferTexture());*/

	//=========================================================================================
	// オフスクリーン描画処理
	//=========================================================================================

	{
		/* debugScreen */
		MyEngine::BeginOffscreen(console->GetSceneTexture());
		MyEngine::camera3D_ = console->GetDebugCamera();

		bunny_->Draw();
		/*particle_->Draw();*/

		MyEngine::EndOffscreen(console->GetSceneTexture());
		MyEngine::TransitionProcess();

		/* defferedRendering */
		MyEngine::BeginOffscreens(kCountOfDefferedRenderingType, defferedRendering_->GetTexturePtrs());

		particle_->Draw();
		nodeModel_->Draw();

		MyEngine::EndOffscreens(kCountOfDefferedRenderingType, defferedRendering_->GetTexturePtrs());
		MyEngine::TransitionProcess();
		console->OutputDefferedTextures("Deffered", DefferedRenderingType::kCountOfDefferedRenderingType, defferedRendering_->GetTextureHandles());

		/* BloomRendering */
		MyEngine::BeginOffscreen(bloom_->GetRenderTargetTexture());

		defferedRendering_->Draw(MyEngine::camera3D_->GetGPUVirtualAddress(), light_->GetGPUVirtualAddress());

		MyEngine::EndOffscreen(bloom_->GetRenderTargetTexture());
		MyEngine::TransitionProcess();
		console->OutputTexture("bloom:renderTarget", bloom_->GetRenderTargetTexture()->GetGPUHandleSRV());

		bloom_->CreateBloom();

		/* main screen */
		MyEngine::BeginOffscreen(MyEngine::GetTexture("offscreen"));
		bloom_->Draw();
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
