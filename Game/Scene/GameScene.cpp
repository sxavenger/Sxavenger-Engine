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

	console_->Init();
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

		console_->Update();
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
	console_->Term();

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
	teapot_ = std::make_unique<Teapot>();
	glass_  = std::make_unique<Glass>();
	bunny_ = std::make_unique<Bunny>();
	player_ = std::make_unique<Player>();
	room_ = std::make_unique<Room>();
	ocean_ = std::make_unique<Ocean>();


	// TLASの生成
	{ // todo: worldMatrixが変わるので関数にしておく

		uint32_t baseDescIndex = 6;

		InstanceDesc desc = {};
		desc.Resize(baseDescIndex + glass_->GetModelSize());

		desc.Set(0, ground_->GetBlas(), ground_->GetWorldMatrix(), 0);
		desc.Set(1, teapot_->GetBlas(), teapot_->GetWorldMatrix(), 0);
		desc.Set(2, bunny_->GetBlas(), bunny_->GetWorldMatrix(), 0);
		desc.Set(3, player_->GetBlas(), player_->GetWorldMatrix(), 0);
		desc.Set(4, room_->GetBlas(), room_->GetWorldMatrix(), 0);
		desc.Set(5, ocean_->GetBlas(), ocean_->GetWorldMatrix(), 0);
		
		for (uint32_t i = 0; i < glass_->GetModelSize(); ++i) {

			uint32_t descIndex = baseDescIndex + i;

			desc.Set(
				descIndex, glass_->GetBlas(i), glass_->GetWorldMatrix(), i
			);
		}
		
		tlas_ = std::make_unique<TopLevelAS>();
		tlas_->Create(desc);
	}

	RayTracingEngine::CreateStateObject(
		kWindowWidth, kWindowHeight,
		tlas_.get()
	);
}

void GameScene::Term() {
}

void GameScene::Update() {
	player_->Update();
	camera_->Update(player_->GetWorldMatrix());
	bunny_->Update();

	// TLASの更新
	{
		uint32_t baseDescIndex = 6;

		InstanceDesc desc = {};
		desc.Resize(baseDescIndex + glass_->GetModelSize());

		desc.Set(0, ground_->GetBlas(), ground_->GetWorldMatrix(), 0);
		desc.Set(1, teapot_->GetBlas(), teapot_->GetWorldMatrix(), 0);
		desc.Set(2, bunny_->GetBlas(), bunny_->GetWorldMatrix(), 0);
		desc.Set(3, player_->GetBlas(), player_->GetWorldMatrix(), 0);
		desc.Set(4, room_->GetBlas(), room_->GetWorldMatrix(), 0);
		desc.Set(5, ocean_->GetBlas(), ocean_->GetWorldMatrix(), 0);

		for (uint32_t i = 0; i < glass_->GetModelSize(); ++i) {

			uint32_t descIndex = baseDescIndex + i;

			desc.Set(
				descIndex, glass_->GetBlas(i), glass_->GetWorldMatrix(), i
			);
		}

		tlas_->Update(desc);
	}
}

void GameScene::Draw() {

	auto commandList = MyEngine::GetCommandList();

	//=========================================================================================
	// レイトレーシング描画処理
	//=========================================================================================

	{
		RayTracingEngine::BeginRayTracing();

		commandList->SetComputeRootDescriptorTable(0, tlas_->GetGPUDescriptorHandle());
		commandList->SetComputeRootConstantBufferView(1, camera_->GetGPUVirtualAddress());
		commandList->SetComputeRootConstantBufferView(2, light_->GetGPUVirtualAddress());
		commandList->SetComputeRootDescriptorTable(3, glass_->GetGPUHandle());
		commandList->SetComputeRootDescriptorTable(4, MyEngine::GetTextureHandleGPU("resources/tile_black.png"));

		RayTracingEngine::EndRayTracing();
	}

	MyEngine::TransitionProcess();
	console_->OutputRayTracingResult(RayTracingEngine::GetDxrCommon()->GetResultBufferTexture());

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
