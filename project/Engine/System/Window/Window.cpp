#include "Window.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Window class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Window::Create(const Vector2ui& clientSize, const LPCWSTR name, const HWND parentHwnd) {

	// 引数の保存
	clientSize_ = clientSize;
	name_       = name;
	className_  = L"Sxavenger Engine Window: ";
	className_ += name;

	// window type の設定
	type_ = WindowType::kMainWindow;

	if (parentHwnd != nullptr) {
		type_ = WindowType::kSubWindow;
	}

	// インスタンスハンドルを取得
	hInst_ = GetModuleHandle(nullptr);
	Assert(hInst_ != nullptr);

	// window設定
	WNDCLASS wc = {};
	wc.lpszClassName = className_.c_str();
	wc.hInstance     = hInst_;
	wc.lpfnWndProc   = MainWindowProc;

	if (type_ == WindowType::kSubWindow) {
		wc.lpfnWndProc = SubWindowProc;
	}

	Assert(RegisterClass(&wc));

	RECT rc = {};
	rc.right  = clientSize_.x;
	rc.bottom = clientSize_.y;

	// ウィンドウサイズの調整
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウを生成
	hwnd_ = CreateWindow(
		wc.lpszClassName,
		name_,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, //!< windowのサイズの固定
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		parentHwnd,
		nullptr,
		hInst_,
		nullptr
	);
	Assert(hwnd_ != nullptr);

	// ウィンドウを表示
	ShowWindow(hwnd_, SW_SHOW);
}

void Window::Close() {
	DestroyWindow(hwnd_);
	CloseWindow(hwnd_);

	if (hInst_ != nullptr) {
		UnregisterClass(className_.c_str(), hInst_);
	}
}

void Window::SetIcon(const std::filesystem::path& filepath, const Vector2ui& cursolSize) {
	SetWindowIcon(filepath, cursolSize);
	SetTaskbarIcon(filepath, cursolSize);
}

void Window::SetWindowIcon(const std::filesystem::path& filepath, const Vector2ui& cursolSize) {

	HICON smallIcon
		= static_cast<HICON>(LoadImageA(GetModuleHandle(NULL), filepath.generic_string().c_str(), IMAGE_ICON, cursolSize.x, cursolSize.y, LR_LOADFROMFILE));

	SendMessage(hwnd_, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(smallIcon));
}

void Window::SetTaskbarIcon(const std::filesystem::path& filepath, const Vector2ui& cursolSize) {

	HICON largeIcon
		= static_cast<HICON>(LoadImageA(GetModuleHandle(NULL), filepath.generic_string().c_str(), IMAGE_ICON, cursolSize.x, cursolSize.y, LR_LOADFROMFILE));

	SendMessage(hwnd_, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(largeIcon));
}

LRESULT Window::MainWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch (msg) {
		case WM_DESTROY: //!< ウィンドウが破棄された
			// OSに対して, アプリの終了を伝える
			PostQuitMessage(0);
			return 0;
			break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

LRESULT Window::SubWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch (msg) {
		case WM_CLOSE: //!< windowが終了した
			// windowの破棄
			DestroyWindow(hwnd);
			return 0;
			break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);

}
