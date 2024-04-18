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

// Game
#include <Logger.h>

//
#include "externals/imgui/imgui_internal.h"

////////////////////////////////////////////////////////////////////////////////////////////
// メイン関数
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//=========================================================================================
	// 初期化
	//=========================================================================================
	MyEngine::Initialize(kWindowWidth, kWindowHeight, kWindowTitle);

	/// Console class にまとめる
	static bool isOpenWindow = true;

	////////////////////////////////////////////////////////////////////////////////////////////
	// メインループ
	////////////////////////////////////////////////////////////////////////////////////////////
	while (MyEngine::ProcessMessage() == 0) {

		MyEngine::GetDxCommon()->BeginOffscreen();

		// offscreen Update


		MyEngine::GetDxCommon()->EndOffscreen();

		MyEngine::BeginFrame();

		//=========================================================================================
		// 更新処理
		//=========================================================================================

		ImGui::Begin("system");
		ImGui::Text("speed(s): %.6f", ExecutionSpeed::freamsParSec_);
		ImGui::Text("FPS: %.1f", 1.0f / ExecutionSpeed::freamsParSec_);
		ImGui::End();

		ImGui::Begin("game");
		ImGui::End();

		// コンソール
		ImGui::Begin("console", &isOpenWindow, ImGuiWindowFlags_NoCollapse);

		//タブ等を除いたウィンドウのサイズを取得(計算)
		ImVec2 cntRegionMax = ImGui::GetWindowContentRegionMax();
		ImVec2 cntRegionMin = ImGui::GetWindowContentRegionMin();
		ImVec2 wndSize = { cntRegionMax.x - cntRegionMin.x, cntRegionMax.y - cntRegionMin.y };

		//元のアス比とImGuiウィンドウのアス比を比較
		float outerWindowAspectRatio = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight);
		float innerWindowAspectRatio = wndSize.x / wndSize.y;
		ImVec2 finalWndSize = wndSize;

		//横幅が大きかったら縦基準で画像サイズを決定
		if (outerWindowAspectRatio <= innerWindowAspectRatio) {
			finalWndSize.x *= outerWindowAspectRatio / innerWindowAspectRatio;
		}
		//縦幅が大きかったら横基準で画像サイズを決定
		else {
			finalWndSize.y *= innerWindowAspectRatio / outerWindowAspectRatio;
		}

		//画像を中央に持ってくる
		ImVec2 topLeft = { (wndSize.x - finalWndSize.x) * 0.5f + cntRegionMin.x,
							(wndSize.y - finalWndSize.y) * 0.5f + cntRegionMin.y };
		ImGui::SetCursorPos(topLeft);

		//!< ここのTextureHandleがRTVになればOK!
		D3D12_GPU_DESCRIPTOR_HANDLE handle = MyEngine::GetTextureHandleGPU("offscreen");
		ImGui::Image((ImTextureID)handle.ptr, finalWndSize);

		ImGui::End();

		//=========================================================================================
		// 描画処理
		//=========================================================================================
		
		MyEngine::EndFrame();

		//*****************************************************************************************
		// 今の BeginFrame ~ EndFrame を offscreenの書き込みコマンドにする。
		// その後EndFrameでRenderTargetViewのTextureを生成. メモリ上でいい感じにTextureにする
		// 詳細 >>
		// 
		// 最後にRTVのTextureをScreenに描画して1frame終了
		//*****************************************************************************************

	}

	MyEngine::Finalize();
	
	return 0;

}