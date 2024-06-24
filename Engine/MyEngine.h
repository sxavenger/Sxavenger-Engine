#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <windows.h>
#include <Input.h>

// c++
#include <cstdint>
#include <string>
#include <unordered_map>

// DirectXCommon
#include <DirectXRCommon.h>

// Camera
#include <Camera3D.h>
#include <Camera2D.h>

namespace Sxavenger {

}

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class DirectXRCommon;
class DirectXCommon;
class Texture;
class TextureManager;

////////////////////////////////////////////////////////////////////////////////////////////
// MyEngine class
////////////////////////////////////////////////////////////////////////////////////////////
class MyEngine {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 初期化処理
	//! 
	//! @param[in] width        ウィンドウ横幅
	//! @param[in] height       ウィンドウ縦幅
	//! @param[in] kWindowTitle ウィンドウタイトル
	static void Initialize(int32_t kWindowWidth, int32_t kWindowHeight, const char* kWindowTitle);

	//! @brief 終了処理
	static void Finalize();

	//! @brief フレームの開始
	static void BeginFrame();

	//! @brief フレームの終了
	static void EndFrame();

	//! @brief 描画処理の開始
	static void BeginDraw();

	//! @brief スクリーン描画処理の開始
	static void BeginScreenDraw();

	//! @brief オフスク描画処理の開始
	//! 
	//! @param[in] offscreenRenderTexture 書き込む用のdummyTexture
	static void BeginOffscreen(Texture* renderTexture);

	//! @brief オフスク描画処理の開始
	static void EndOffscreen(Texture* renderTexture);

	static void BeginOffscreens(Texture* renderTextures[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]);

	static void EndOffscreens(Texture* renderTextures[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]);

	static void TransitionProcess();

	static void TransitionProcessSingle();

	//! @brief プロセスメッセージを取得
	//! 
	//! @retval 1 ゲーム終了
	//! @retval 0 ゲーム継続
	static int ProcessMessage();

	//-----------------------------------------------------------------------------------------
	// pipeline関係
	//-----------------------------------------------------------------------------------------



	//-----------------------------------------------------------------------------------------
	// descriptor関係
	//-----------------------------------------------------------------------------------------

	static DxObject::Descriptor GetCurrentDescripor(DxObject::DescriptorType type);

	static void EraseDescriptor(DxObject::Descriptor& descriptor);

	//-----------------------------------------------------------------------------------------
	// texture関係
	//-----------------------------------------------------------------------------------------

	static TextureManager* GetTextureManager();

	static Texture* CreateRenderTexture(const std::string& key, int32_t textureWidth, int32_t textureHeight, const Vector4f& clearColor = defaultClearColor);

	static Texture* LoadTexture(const std::string& filePath);

	static Texture* GetTexture(const std::string& textureKey);

	static const D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureHandleGPU(const std::string& textureKey);


	//-----------------------------------------------------------------------------------------
	// Input関係
	//-----------------------------------------------------------------------------------------

	static bool IsPressKey(uint8_t dik);

	static bool IsTriggerKey(uint8_t dik);

	static bool IsReleaseKey(uint8_t dik);

	//-----------------------------------------------------------------------------------------
	// DxSystem関係
	//-----------------------------------------------------------------------------------------

	static ID3D12Device8* GetDevice();

	// TODO: あんましたくない
	static ID3D12GraphicsCommandList6* GetCommandList();

	static DxObject::Devices* GetDevicesObj();
	static DirectXCommon* GetDxCommon();

	

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	static Camera3D* camera3D_;
	static Camera2D* camera2D_;

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// RayTracingEngine class
////////////////////////////////////////////////////////////////////////////////////////////
class RayTracingEngine
	: public MyEngine {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void BeginRayTracing(DxrObject::TopLevelAS* tlas);

	static void EndRayTracing();

	static DirectXRCommon* GetDxrCommon();

private:
};