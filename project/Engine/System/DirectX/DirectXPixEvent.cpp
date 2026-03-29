#include "DirectXPixEvent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

//* c++
#include <stack>
#include <ranges>

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

void DirectXPixEvent::Init() {
	DirectXPixEvent::InitLibraryDirectory();

	//!< PIXのHUDを無効化.
	SetEnvironmentVariableA("PIXDisableHUD", "1");

	//!< moduleの読み込み. (DirectX12の生成前に行う)
	DirectXPixEvent::LoadModule(L"WinPixGpuCapturer.dll");
}

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
		StreamLogger::EngineLog("[DirectXPixEvent] error | capture failed. invalid file extension.");
		return; //!< 拡張子が.wpixでない場合は何もしない
	}


#ifdef USE_PIX
	//!< [pix3.h] 自作関数PIXGpuCaptureNextFramesImplを呼び出す. (WinPixGpuCapturer.dllのCaptureNextFrame関数を呼び出す)
	
	auto hr = DirectXPixEvent::PIXGpuCaptureNextFramesImpl(filepath, frames);
	if (FAILED(hr)) {
		StreamLogger::EngineLog(std::format(L"[DirectXPixEvent] error | capture failed. _com_error: {}", DxObject::GetComError(hr)));
		return;
	}
	
	StreamLogger::EngineLog(std::format("[DirectXPixEvent] pix captured. filepath: {}", filepath.generic_string()));
#else
	//!< [pix.h] pix.hにはGPUキャプチャのAPIがないため、何もしない
#endif
}

void DirectXPixEvent::InitLibraryDirectory() {
#ifdef USE_PIX
	static const std::filesystem::path kPixDirectory   = L"C:/Program Files/Microsoft PIX";
	static const std::filesystem::path kPixApplication = L"WinPix.exe";
	//!< kPixDirectory / <version> / kPixApplication の有効versionを探す. (WinPix.exeが存在する場所を指定する)

	if (!std::filesystem::exists(kPixDirectory)) {
		StreamLogger::EngineLog(std::format("[DirectXPixEvent] warning | pix version directory not found. directory: {}", kPixDirectory.generic_string()));
		return;
	}

	std::stack<std::filesystem::path> version;

	for (const auto& entry : std::filesystem::directory_iterator(kPixDirectory) | std::views::filter([](const auto& entry) { return entry.is_directory(); })) { //!< directoryのみを対象.
		//!< 古いversionから順に探索するため、stackにpushする. (stackのtopが最新versionになる)
		version.emplace(entry.path());
	}

	while (!version.empty()) {
		std::filesystem::path current = version.top();
		version.pop();

		if (std::filesystem::exists(current / kPixApplication)) {
			libraryDirectory = current;
			StreamLogger::EngineLog(std::format("[DirectXPixEvent] pix enable version directory found. directory: {}", libraryDirectory.generic_string()));
			return; //!< WinPix.exeが存在するversionが見つかった時点で探索を終了する.
		}
	}

	StreamLogger::EngineLog(std::format(L"[DirectXPixEvent] warning | pix enable version directory not found."));

#endif
}

void DirectXPixEvent::LoadModule(const std::filesystem::path& filename) {
#ifdef USE_PIX
	//!< [pix3.h] moduleを読み込み

	if (libraryDirectory.empty()) {
		return; //!< libraryDirectoryが見つかっていない場合は何もしない.
	}
	
	const std::filesystem::path filepath = libraryDirectory / filename;

	HMODULE module = GetModuleHandleW(filepath.generic_wstring().c_str());

	if (module != NULL) {
		return; //!< 既に読み込まれている場合は何もしない.
	}

	//!< LoadLibraryWでdllを読み込む.
	LoadLibraryW(filepath.generic_wstring().c_str());

#endif
}

void* DirectXPixEvent::GetGpuCaptureFunctionPointer(const std::string& function) {
#ifdef USE_PIX
	//!< [pix3.h] PixImpl::GetGpuCaptureFunctionPtr関数を参照.

	if (libraryDirectory.empty()) {
		return nullptr; //!< libraryDirectoryが見つかっていない場合はnullptrを返す.
	}

	const std::filesystem::path filepath = libraryDirectory / L"WinPixGpuCapturer.dll";
	//!< 事前にlibraryDirectoryに格納されたversionのWinPixGpuCapturer.dllを対象とする.

	HMODULE module = GetModuleHandleW(filepath.generic_wstring().c_str());
	if (module == NULL) {
		return nullptr; //!< moduleが読み込まれていない場合はnullptrを返す.
	}

	auto fn = (void*)GetProcAddress(module, function.c_str());
	if (fn == nullptr) {
		return nullptr;
	}

	return fn;

#else
	return nullptr; //!< [pix.h] pix.hにはGPUキャプチャのAPIがないため、常にnullptrを返す
#endif
}

HRESULT WINAPI DirectXPixEvent::PIXGpuCaptureNextFramesImpl(const std::filesystem::path& filepath, uint32_t frames) {
#ifdef USE_PIX
	//!< [pix3.h] PIXGpuCaptureNextFrames関数を参照.

	typedef HRESULT(WINAPI* CaptureNextFrameFn)(PCWSTR, UINT32);

	auto fn = (CaptureNextFrameFn)DirectXPixEvent::GetGpuCaptureFunctionPointer("CaptureNextFrame");
	if (fn == nullptr) {
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return fn(filepath.generic_wstring().c_str(), frames);
	
#else
	return E_NOTIMPL; //!< [pix.h] pix.hにはGPUキャプチャのAPIがないため、常にE_NOTIMPLを返す
#endif
}
