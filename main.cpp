#include <MyEngine.h> // sxavenger engine
#include "DirectXCommon.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Console
#include <Console.h>
/// lib
#include <Environment.h>
#include <ExecutionSpeed.h>
// Geometry
#include <Vector4.h>
#include <Matrix4x4.h>
#include <Vector2.h>

// Light
#include <Light.h>

// c++
#include <list>
#include <memory>

#include <Logger.h>

// Game
#include "Game/Floor/Floor.h"

#include <VectorComparison.h>

////////////////////////////////////////////////////////////////////////////////////////////
// メイン関数
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//=========================================================================================
	// 初期化
	//=========================================================================================
	MyEngine::Initialize(kWindowWidth, kWindowHeight, kWindowTitle);
	Console::GetInstance()->Init();

	std::unique_ptr<Floor> floor = std::make_unique<Floor>();
	
	std::unique_ptr<Camera3D> camera = std::make_unique<Camera3D>();
	MyEngine::camera3D_ = camera.get();

	MyEngine::SetWriteTexture(MyEngine::GetTexture("offscreen"), { camera.get() });

	////////////////////////////////////////////////////////////////////////////////////////////
	// メインループ
	////////////////////////////////////////////////////////////////////////////////////////////
	while (MyEngine::ProcessMessage() == 0) {

		MyEngine::BeginFrame();

		//=========================================================================================
		// 更新処理
		//=========================================================================================

		static int frame = 0;
		++frame;

		if (frame % 60 == 0) {
			Console::GetInstance()->SetLog(
				std::format("[debug] frame: {}, speed: {}", frame / 60, ExecutionSpeed::freamsParSec_),
				Console::commentOutColor
			);
		}

		Console::GetInstance()->Update();

		

		//=========================================================================================
		// オフスクリーン描画処理
		//=========================================================================================

		// MyEngineに登録されているdummyTexture分回す
		for (auto& info : MyEngine::GetWriteTextures()) {
			MyEngine::BeginOffScreen(info.first);
			MyEngine::offscreenInfo_ = &info.second;

			floor->Update(); // HACK: Updateを二回行うことになる.

			floor->Draw();

			MyEngine::EndOffScreen();
		}

		//=========================================================================================
		// スクリーン描画処理
		//=========================================================================================

		MyEngine::BeginDraw();



		MyEngine::EndFrame();

	}

	floor.reset();
	MyEngine::Finalize();
	
	return 0;

}