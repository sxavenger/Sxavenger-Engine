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
class DirectXPixEvent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// HUDOptions enum class [PIXHUDOptions参照]
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class HUDOptions : uint8_t {
		ShowOnAllWindows       = 0x1,
		ShowOnTargetWindowOnly = 0x2,
		ShowOnNoWindows        = 0x4
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void BeginEvent(ID3D12GraphicsCommandList* commandList, const std::wstring& name, uint8_t indent);

	static void EndEvent(ID3D12GraphicsCommandList* commandList);

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
