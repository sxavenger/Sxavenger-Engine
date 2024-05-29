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

#include "DirectXRCommon.h"
#include "Lib/PostEffect/GaussianBlur.h"

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

	MyEngine::TransitionProcessSingle();
	MyEngine::EnableTextures();

	std::unique_ptr<GaussianBlur> blur = std::make_unique<GaussianBlur>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// メインループ
	////////////////////////////////////////////////////////////////////////////////////////////
	while (MyEngine::ProcessMessage() == 0) {

		MyEngine::BeginFrame();

		//=========================================================================================
		// 更新処理
		//=========================================================================================

		{
			/*static int frame = 0;
			++frame;*/

			/*if (frame % 60 == 0) {
				console->SetLog(
					std::format("[debug] frame: {}, speed(ms): {}", frame / 60, Performance::GetFramesPerformance(SecondsUnit::ms)),
					Console::commentOutColor
				);
			}*/

			console->Update();

		}

		MyEngine::TransitionProcess();
		MyEngine::BeginDraw();

		//=========================================================================================
		// レイトレーシング描画処理
		//=========================================================================================

		{
			RayTracingEngine::GetDxrCommon()->DrawRayTracing();
		}
		MyEngine::TransitionProcess();
		console->OutputRayTracingResult(RayTracingEngine::GetDxrCommon()->GetResultBufferTexture());

		//-----------------------------------------------------------------------------------------
		// postEffect test
		//-----------------------------------------------------------------------------------------

		{
			blur->CreateBlurTexture(kWindowWidth, kWindowHeight, RayTracingEngine::GetDxrCommon()->GetResultBufferTexture());
		}
		MyEngine::TransitionProcess();
		console->OutputTexture("blur", blur->GetTexture());

		//=========================================================================================
		// オフスクリーン描画処理
		//=========================================================================================
		
		{
			// todo: 複数へのtexture書き込みをさせる.
			MyEngine::BeginOffScreen(console->GetSceneTexture());
			MyEngine::camera3D_ = console->GetDebugCamera();

			RayTracingEngine::GetDxrCommon()->DrawRasterlize();

			MyEngine::EndOffScreen();


			MyEngine::BeginOffScreen(MyEngine::GetTexture("offscreen"));
			MyEngine::camera3D_ = camera.get();



			MyEngine::EndOffScreen();
		}

		MyEngine::TransitionProcess();

		//=========================================================================================
		// スクリーン描画処理
		//=========================================================================================

		{
			MyEngine::BeginScreenDraw();

			/*
				ImGuiの関係上、スクリーン描画は最後にする
			*/
		}

		MyEngine::EndFrame();

	}

	console->Term();
	blur.reset();

	MyEngine::Finalize();
	return 0;

}