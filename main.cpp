#include <MyEngine.h> // sxavenger engine
#include <DirectXCommon.h>

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
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

////////////////////////////////////////////////////////////////////////////////////////////
// メイン関数
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//=========================================================================================
	// 初期化
	//=========================================================================================
	MyEngine::Initialize(kWindowWidth, kWindowHeight, kWindowTitle);

	/*ID3D12GraphicsCommandList* commandList = MyEngine::GetCommandList();*/

	//-----------------------------------------------------------------------------------------
	// Camera
	//-----------------------------------------------------------------------------------------
	std::unique_ptr<Camera3D> camera3D = std::make_unique<Camera3D>("camera3D.json");
	MyEngine::camera3D_ = camera3D.get();
	std::unique_ptr<Camera2D> camera2D = std::make_unique<Camera2D>();
	MyEngine::camera2D_ = camera2D.get();

	////////////////////////////////////////////////////////////////////////////////////////////
	// メインループ
	////////////////////////////////////////////////////////////////////////////////////////////
	while (MyEngine::ProcessMessage() == 0) {

		MyEngine::BeginFrame();

		//=========================================================================================
		// 更新処理
		//=========================================================================================
		camera3D->UpdateImGui();

		ImGui::Begin("system");
		ImGui::Text("speed(s): %.6f", ExecutionSpeed::freamsParSec_);
		ImGui::Text("FPS: %.1f", 1.0f / ExecutionSpeed::freamsParSec_);
		ImGui::End();



		//=========================================================================================
		// 描画処理
		//=========================================================================================


		
		MyEngine::EndFrame();
	}

	// camer
	camera3D.reset();
	camera2D.reset();

	MyEngine::Finalize();
	
	return 0;

}