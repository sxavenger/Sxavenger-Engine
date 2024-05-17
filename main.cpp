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
#include "Floor.h"
#include "Plane.h"
#include "Sphere.h"
#include "Cube.h"

#include <Input.h>

////////////////////////////////////////////////////////////////////////////////////////////
// メイン関数
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//=========================================================================================
	// 初期化
	//=========================================================================================
	MyEngine::Initialize(kWindowWidth, kWindowHeight, kWindowTitle);

	std::unique_ptr<Camera3D> camera = std::make_unique<Camera3D>();
	MyEngine::camera3D_ = camera.get();

	auto console = Console::GetInstance();
	console->Init();

	std::unique_ptr<Floor> floor = std::make_unique<Floor>();
	std::unique_ptr<Plane> plane = std::make_unique<Plane>();
	std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
	std::unique_ptr<Cube> cube = std::make_unique<Cube>();

	//////////////////////////////////////////////////////////////////////////////////////////////
	//// メインループ
	//////////////////////////////////////////////////////////////////////////////////////////////
	while (MyEngine::ProcessMessage() == 0) {

		MyEngine::BeginFrame();

		//=========================================================================================
		// 更新処理
		//=========================================================================================

		/*static int frame = 0;
		++frame;*/

		/*if (frame % 60 == 0) {
			console->SetLog(
				std::format("[debug] frame: {}, speed(ms): {}", frame / 60, Performance::GetFramesPerformance(SecondsUnit::ms)),
				Console::commentOutColor
			);
		}*/

		console->Update();

		floor->Update();
		plane->Update();
		sphere->Update();
		cube->Update();

		MyEngine::BeginDraw();

		//=========================================================================================
		// オフスクリーン描画処理
		//=========================================================================================

		// todo: 複数へのtexture書き込みをさせる.
		MyEngine::BeginOffScreen(MyEngine::GetTexture("offscreen"));
		MyEngine::camera3D_ = camera.get();

		plane->Draw();
		floor->Draw();
		sphere->Draw();
		cube->Draw();

		MyEngine::EndOffScreen();

		MyEngine::BeginOffScreen(console->GetSceneTexture());
		MyEngine::camera3D_ = console->GetDebugCamera();

		plane->Draw();
		floor->Draw();
		sphere->Draw();
		cube->Draw();

		MyEngine::EndOffScreen();
		

		//=========================================================================================
		// スクリーン描画処理
		//=========================================================================================

		MyEngine::BeginScreenDraw();



		MyEngine::EndFrame();

	}

	console->Term();
	camera.reset();

	floor.reset();
	plane.reset();
	sphere.reset();
	cube.reset();

	MyEngine::Finalize();
	
	return 0;

}