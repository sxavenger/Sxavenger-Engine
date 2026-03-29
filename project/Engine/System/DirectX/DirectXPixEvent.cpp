#include "DirectXPixEvent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

//* pix
#ifdef _DEVELOPMENT
//* externals
#define USE_PIX
#include <PixEvents/pix3.h>
#else
#include <pix.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXPixEvent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectXPixEvent::BeginEvent(ID3D12GraphicsCommandList* commandList, const std::wstring& name, uint8_t indent) {
#ifdef USE_PIX
	PIXBeginEvent(commandList, PIX_COLOR_INDEX(indent), name.c_str()); //!< [pix3.h]
#else
	PIXBeginEvent(commandList, NULL, name.c_str()); //!< [pix.h]
#endif
}

void DirectXPixEvent::EndEvent(ID3D12GraphicsCommandList* commandList) {
#ifdef USE_PIX
	PIXEndEvent(commandList); //!< [pix3.h]
#else
	PIXEndEvent(commandList); //!< [pix.h]
#endif
}

void DirectXPixEvent::CaptureNextFrames(const std::filesystem::path& filepath, uint32_t frames) {
	if (filepath.extension() != L".wpix") {
		StreamLogger::EngineLog("[DirectXPixEvent] error | Capture failed. invalid file extension.");
		return; //!< 拡張子が.wpixでない場合は何もしない
	}

#ifdef USE_PIX
	auto hr = PIXGpuCaptureNextFrames(filepath.generic_wstring().c_str(), frames); //!< [pix3.h]

	if (FAILED(hr)) {
		StreamLogger::EngineLog(std::format(L"[DirectXPixEvent] error | Capture failed. _com_error: {}", DxObject::GetComError(hr)));
		return;
	}
	
	StreamLogger::EngineLog(std::format("[DirectXPixEvent] Pix Capture. filepath: {}", filepath.generic_string()));
#else
	//!< [pix.h] pix.hにはGPUキャプチャのAPIがないため、何もしない
#endif
}
