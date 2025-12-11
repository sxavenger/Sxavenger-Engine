#include "WinApp.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include "../Utility/ComPtr.h"
#include "../Utility/StreamLogger.h"

//* windows
#include <shobjidl.h>
#include <shellapi.h>
#include <shlobj.h>

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

bool WinApp::OpenExplorer(const std::filesystem::path& filepath) {

	PIDLIST_ABSOLUTE pidl = nullptr;

	HRESULT hr = SHParseDisplayName(
		std::filesystem::absolute(filepath).c_str(),
		nullptr,
		&pidl,
		0,
		nullptr
	);

	if (FAILED(hr) || pidl == nullptr) {
		return false;
	}

	hr = SHOpenFolderAndSelectItems(
		pidl,
		0,
		nullptr,
		0
	);

	CoTaskMemFree(pidl);
	return SUCCEEDED(hr);

}

bool WinApp::OpenApplication(const std::filesystem::path& filepath) {

	HINSTANCE result = ShellExecuteW(
		nullptr,
		L"open",
		filepath.c_str(),
		nullptr,
		nullptr,
		SW_SHOWNORMAL
	);

	return reinterpret_cast<intptr_t>(result) > 32;
	
}
