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
	type_ = Category::MainWindow;

	if (parentHwnd != nullptr) {
		type_ = Category::SubWindow;
	}

	// インスタンスハンドルを取得
	hInst_ = GetModuleHandle(nullptr);
	Exception::Assert(hInst_ != nullptr);

	// window設定
	WNDCLASS wc = {};
	wc.lpszClassName = className_.c_str();
	wc.hInstance     = hInst_;
	wc.lpfnWndProc   = MainWindowProc;

	if (type_ == Category::SubWindow) {
		wc.lpfnWndProc = SubWindowProc;
	}

	Exception::Assert(RegisterClass(&wc));

	rect_ = {};
	rect_.right  = clientSize_.x;
	rect_.bottom = clientSize_.y;

	// ウィンドウサイズの調整
	AdjustWindowRect(&rect_, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウを生成
	hwnd_ = CreateWindow(
		wc.lpszClassName,
		name_,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, //!< windowのサイズの固定
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect_.right - rect_.left,
		rect_.bottom - rect_.top,
		parentHwnd,
		nullptr,
		hInst_,
		nullptr
	);
	Exception::Assert(hwnd_ != nullptr);

	// ウィンドウを表示
	ShowWindow(hwnd_, SW_SHOW);
}

void Window::CreateEx(const Vector2ui& clientSize, const LPCWSTR name, const WNDPROC& proc, const HWND parentHwnd) {

	// 引数の保存
	clientSize_ = clientSize;
	name_       = name;
	className_  = L"Sxavenger Engine Window: ";
	className_ += name;

	// window type の設定
	type_ = Category::MainWindow;

	if (parentHwnd != nullptr) {
		type_ = Category::SubWindow;
	}

	// インスタンスハンドルを取得
	hInst_ = GetModuleHandle(nullptr);
	Exception::Assert(hInst_ != nullptr);

	// window設定
	WNDCLASS wc = {};
	wc.lpszClassName = className_.c_str();
	wc.hInstance     = hInst_;
	wc.lpfnWndProc   = proc;

	Exception::Assert(RegisterClass(&wc));

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
	Exception::Assert(hwnd_ != nullptr);

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

void Window::SetWindowMode(Mode mode) {
	if (mode_ == mode) {
		return; //!< 既に設定されている
	}

	uint32_t style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	// todo: styleを指定できるように変更

	switch (mode) {
		case Mode::Borderless:
			{
				// 元の状態を取得
				GetWindowRect(hwnd_, &rect_);

				// 仮想フルスクリーン化
				SetWindowLong(
					hwnd_,
					GWL_STYLE,
					style & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME)
				);

				// モニター情報の取得
				HMONITOR monitor = MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONEAREST);
				MONITORINFO info = {};
				info.cbSize = sizeof(MONITORINFO);
				GetMonitorInfo(monitor, &info);

				RECT rc = {};
				rc.right  = info.rcMonitor.right - info.rcMonitor.left;
				rc.bottom = info.rcMonitor.bottom - info.rcMonitor.top;

				// ウィンドウの位置を変更
				SetWindowPos(
					hwnd_, HWND_TOPMOST,
					rc.left, rc.top, rc.right, rc.bottom,
					SWP_NOACTIVATE
				);

				ShowWindow(hwnd_, SW_MAXIMIZE);
			}
			break;

		case Mode::Window:
			{
				// 通常ウィンドウに戻す
				SetWindowLong(hwnd_, GWL_STYLE, style);

				// ウィンドウの位置を変更
				RECT rc = {};
				rc.left   = rect_.left;
				rc.top    = rect_.top;
				rc.right  = rect_.right - rect_.left;
				rc.bottom = rect_.bottom - rect_.top;

				SetWindowPos(
					hwnd_, HWND_NOTOPMOST,
					rc.left, rc.top, rc.right, rc.bottom,
					SWP_FRAMECHANGED | SWP_NOACTIVATE
				);

				ShowWindow(hwnd_, SW_NORMAL);
			}
			break;
	}

	mode_ = mode;
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
