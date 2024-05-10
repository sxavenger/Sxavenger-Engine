#include <MyEngine.h> // sxavenger engine
#include "DirectXCommon.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Console
#include <Console.h>
/// lib
#include <Environment.h>
#include <Performance.h>
// Geometry
#include <Vector4.h>
#include <Matrix4x4.h>
#include <Vector2.h>

// Light
#include <Light.h>

// c++
#include <list>
#include <memory>
#include <array>

#include <Logger.h>

// Game
#include "Game/Floor/Floor.h"
#include "Game/Plane/Plane.h"

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
	std::unique_ptr<Plane> plane = std::make_unique<Plane>();
	
	std::unique_ptr<Camera3D> camera = std::make_unique<Camera3D>();
	MyEngine::camera3D_ = camera.get();

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
				std::format("[debug] frame: {}, speed(ms): {}", frame / 60, Performance::GetFramesPerformance(SecondsUnit::ms)),
				Console::commentOutColor
			);
		}

		Console::GetInstance()->Update();

		floor->Update(); // HACK: Updateを二回行うことになる.
		plane->Update();

		//=========================================================================================
		// オフスクリーン描画処理
		//=========================================================================================

		// todo: 複数へのtexture書き込みをさせる.
		MyEngine::BeginOffScreen(MyEngine::GetTexture("offscreen"));

		plane->Draw();
		floor->Draw();

		MyEngine::EndOffScreen();
		

		//=========================================================================================
		// スクリーン描画処理
		//=========================================================================================

		MyEngine::BeginDraw();



		MyEngine::EndFrame();

	}

	floor.reset();
	plane.reset();
	MyEngine::Finalize();
	
	return 0;

}