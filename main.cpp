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

// Game
#include <Logger.h>

//
#include "externals/imgui/imgui_internal.h"

// Game
#include "Game/Floor/Floor.h"

////////////////////////////////////////////////////////////////////////////////////////////
// メイン関数
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//=========================================================================================
	// 初期化
	//=========================================================================================
	MyEngine::Initialize(kWindowWidth, kWindowHeight, kWindowTitle);
	Console::GetInstance()->Init();

	Floor* floor = new Floor();
	
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
				std::format("[debug] frame: {}, speed: {}", frame / 60, ExecutionSpeed::freamsParSec_),
				Console::commentOutColor
			);
		}

		Console::GetInstance()->Update();

		floor->Update();

		MyEngine::BeginOffScreen();
		// forでscene分回す。

		//=========================================================================================
		// オフスクリーン描画処理
		//=========================================================================================
		
		floor->Draw();

		MyEngine::EndOffScreen();

		MyEngine::BeginDraw();

		//=========================================================================================
		// スクリーン描画処理
		//=========================================================================================
		
		MyEngine::EndFrame();

	}

	delete floor;
	MyEngine::Finalize();
	
	return 0;

}