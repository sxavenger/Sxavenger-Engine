#include "WinApp.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include "../Utility/ComPtr.h"
#include "../Utility/Logger.h"

//* windows
#include <shobjidl.h>

////////////////////////////////////////////////////////////////////////////////////////////
// WinApp class methods
////////////////////////////////////////////////////////////////////////////////////////////

void WinApp::Init() {
	CoInitializeEx(0, COINIT_MULTITHREADED);
	timeBeginPeriod(1);

	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
}

void WinApp::Term() {
	MFShutdown();
	
	CoUninitialize();
}

std::optional<std::filesystem::path> WinApp::GetSaveFilepath(const std::wstring& title, const std::filesystem::path& current, const std::pair<std::wstring, std::wstring>& filter, const std::filesystem::path& extension) {

	ComPtr<IFileSaveDialog> dialog;
	auto hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&dialog));
	Exception::Assert(SUCCEEDED(hr), "WinApp::GetSaveFilename", "failed to create FileSaveDialog instance.");

	dialog->SetTitle(title.c_str());

	ComPtr<IShellItem> folder;
	SHCreateItemFromParsingName(current.wstring().c_str(), nullptr, IID_PPV_ARGS(&folder));
	dialog->SetFolder(folder.Get());

	COMDLG_FILTERSPEC type = { filter.first.c_str(), filter.second.c_str() };
	dialog->SetFileTypes(1, &type);
	dialog->SetDefaultExtension(extension.wstring().c_str());

	hr = dialog->Show(nullptr);
	if (FAILED(hr)) {
		return std::nullopt;
	}

	ComPtr<IShellItem> result;
	dialog->GetResult(&result);

	LPWSTR path = nullptr;
	hr = result->GetDisplayName(SIGDN_FILESYSPATH, &path);

	if (SUCCEEDED(hr)) {
		std::filesystem::path filepath(path);
		CoTaskMemFree(path);
		return filepath;
	}

	return {};

}

std::optional<std::filesystem::path> WinApp::GetOpenFilepath(const std::wstring& title, const std::filesystem::path& current, const std::pair<std::wstring, std::wstring>& filter) {

	ComPtr<IFileOpenDialog> dialog;
	auto hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&dialog));
	Exception::Assert(SUCCEEDED(hr), "WinApp::GetSaveFilename", "failed to create FileOpenDialog instance.");

	dialog->SetTitle(title.c_str());

	ComPtr<IShellItem> folder;
	SHCreateItemFromParsingName(current.wstring().c_str(), nullptr, IID_PPV_ARGS(&folder));
	dialog->SetFolder(folder.Get());

	const COMDLG_FILTERSPEC type = { filter.first.c_str(), filter.second.c_str() };
	dialog->SetFileTypes(1, &type);

	hr = dialog->Show(nullptr);

	if (FAILED(hr)) {
		return std::nullopt;
	}

	ComPtr<IShellItem> result;
	hr = dialog->GetResult(&result);

	LPWSTR path = nullptr;
	hr = result->GetDisplayName(SIGDN_FILESYSPATH, &path);

	if (SUCCEEDED(hr)) {
		std::filesystem::path filepath(path);
		CoTaskMemFree(path);
		return filepath;
	}

	return {};
}
