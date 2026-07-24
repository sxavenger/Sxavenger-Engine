#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT origin
#include "DxObject/DxObjectCommon.h"
#include "DxObject/DxCommandContext.h"

//* engine
#include <Engine/Foundation.h>

//* c++
#include <filesystem>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXPixEvent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief PIX(GPUデバッガ)のイベントマーカー発行とGPUキャプチャを提供するクラス
class DirectXPixEvent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// HUDOptions enum class [PIXHUDOptions参照]
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief PIX HUDの表示対象オプション
	enum class HUDOptions : uint8_t {
		ShowOnAllWindows       = 0x1,
		ShowOnTargetWindowOnly = 0x2,
		ShowOnNoWindows        = 0x4
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief PIXライブラリを読み込み, 初期化する
	static void Init();

	//! @brief デバッグ用のイベント区間を開始する
	//! @param[in] commandList コマンドリスト
	//! @param[in] name        イベント名
	//! @param[in] indent      ネストのインデント(色分け等に使用)
	static void BeginEvent(ID3D12GraphicsCommandList* commandList, const std::wstring& name, uint8_t indent);

	//! @brief デバッグ用のイベント区間を終了する
	//! @param[in] commandList コマンドリスト
	static void EndEvent(ID3D12GraphicsCommandList* commandList);

	//! @brief 次の指定フレーム数をPIXキャプチャしてファイルに保存する
	//! @param[in] filepath 保存先パス
	//! @param[in] frames   キャプチャするフレーム数
	static void CaptureNextFrames(const std::filesystem::path& filepath, uint32_t frames);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline std::filesystem::path libraryDirectory;
	//!< [pix3.h] pixのlibファイルのDirectory. (WinPix.exeが存在する場所を指定する)

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* initialize function *//

	static void InitLibraryDirectory();

	static void LoadModule(const std::filesystem::path& filename);

	//* pix impl function *//

	static void* GetGpuCaptureFunctionPointer(const std::string& function);

	static HRESULT WINAPI PIXGpuCaptureNextFramesImpl(const std::filesystem::path& filepath, uint32_t frames);

	static HRESULT WINAPI PIXSetHUDOptionsImpl(HUDOptions options);

};

SXAVENGER_ENGINE_NAMESPACE_END
