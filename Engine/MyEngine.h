#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <windows.h>

// c++
#include <cstdint>
#include <string>
#include <unordered_map>

// DxObject
#include <DxBlendState.h>
#include <DxPipelineManager.h>

// Camera
#include <Camera3D.h>
#include <Camera2D.h>

namespace Sxavenger {

}

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class DirectXCommon;
class Texture;
class TextureManager;

////////////////////////////////////////////////////////////////////////////////////////////
// structure
////////////////////////////////////////////////////////////////////////////////////////////
struct TextureWriteInfo {
	Camera3D* camera3D = nullptr; //!< カメラ
};

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

	//! @brief オフスク描画処理の開始
	//! 
	//! @param[in] offscreenDummyTexture 書き込む用のdummyTexture
	static void BeginOffScreen(Texture* offscreenDummyTexture);

	//! @brief オフスク描画処理の開始
	static void EndOffScreen();

	//! @brief プロセスメッセージを取得
	//! 
	//! @retval 1 ゲーム終了
	//! @retval 0 ゲーム継続
	static int ProcessMessage();

	// ---- pipeline関係 ---- //

	static void SetPipelineType(PipelineType pipelineType);

	static void SetBlendMode(BlendMode mode);

	static void SetPipelineState();

	// ---- descriptor関係 ---- //

	static DxObject::Descriptor GetCurrentDescripor(DxObject::DescriptorType type);

	static void EraseDescriptor(DxObject::Descriptor& descriptor);

	// ---- texture関係 ---- //

	static TextureManager* GetTextureManager();

	static const D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureHandleGPU(const std::string& textureKey);

	static Texture* GetTexture(const std::string& textureKey);

	// ---- offscreen関係 ---- //

	static void SetWriteTexture(Texture* writeTexture, const TextureWriteInfo& info);

	static void EraseWriteTexture(Texture* writeTexture);

	static const std::unordered_map<Texture*, TextureWriteInfo>& GetWriteTextures();

	// TODO: あんましたくない
	static ID3D12GraphicsCommandList6* GetCommandList();

	static DxObject::Devices* GetDevicesObj();
	static DirectXCommon* GetDxCommon();

	

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	static Camera3D* camera3D_;
	static Camera2D* camera2D_;

	static const TextureWriteInfo* offscreenInfo_;

private:

	//=========================================================================================
	// private varibales
	//=========================================================================================

	static std::unordered_map<Texture*, TextureWriteInfo> writeTextures_;
	//!< key = 書き込むテクスチャのptr, value = 書き込みに必要な構造体

};