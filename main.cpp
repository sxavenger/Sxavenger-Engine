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
// Camera
#include <Camera3D.h>
#include <Camera2D.h>
// Light
#include <Light.h>

// c++
#include <list>
#include <memory>

// Game
#include <Sphere.h>
#include <Terrain.h>

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

	//-----------------------------------------------------------------------------------------
	// Light
	//-----------------------------------------------------------------------------------------
	std::unique_ptr<Light> light = std::make_unique<Light>(MyEngine::GetDevicesObj());

	//-----------------------------------------------------------------------------------------
	// Object
	//-----------------------------------------------------------------------------------------
	std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
	std::unique_ptr<Terrain> terrain = std::make_unique<Terrain>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// メインループ
	////////////////////////////////////////////////////////////////////////////////////////////
	while (MyEngine::ProcessMessage() == 0) {

		MyEngine::BeginFrame();

		//=========================================================================================
		// 更新処理
		//=========================================================================================
		camera3D->UpdateImGui();
		light->UpdateImGui();

		ImGui::Begin("system");
		ImGui::Text("speed(s): %.6f", ExecutionSpeed::freamsParSec_);
		ImGui::Text("FPS: %.1f", 1.0f / ExecutionSpeed::freamsParSec_);
		ImGui::End();

		ImGui::Begin("main");
		sphere->SetOnImGui();
		terrain->SetOnImGui();
		ImGui::End();

		//=========================================================================================
		// 描画処理
		//=========================================================================================
		
		sphere->Draw(light.get());
		terrain->Draw(light.get());

		MyEngine::EndFrame();
	}

	// camer
	camera3D.reset();
	camera2D.reset();
	// light
	light.reset();

	// object
	sphere.reset();
	terrain.reset();

	MyEngine::Finalize();
	
	return 0;

}