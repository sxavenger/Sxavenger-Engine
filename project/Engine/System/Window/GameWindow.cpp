#include "GameWindow.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//* engine
#include <Engine/System/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameWindow class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameWindow::Create(const Vector2ui& clientSize, const LPCWSTR name, const HWND parentHwnd) {

	// 引数の保存
	clientSize_ = clientSize;
	name_       = name;
	className_  = L"Sxavenger Engine Window: ";
	className_ += name;

	if (parentHwnd == nullptr) {
		type_ = GameWindowType::kMainWindow;

	} else {
		type_ = GameWindowType::kSubWindow;
	}

	// インスタンスハンドルを取得
	hInst_ = GetModuleHandle(nullptr);
	Assert(hInst_ != nullptr);

	// window設定
	WNDCLASS wc = {};
	wc.lpszClassName = className_.c_str();
	wc.hInstance     = hInst_;
	wc.lpfnWndProc   = WindowProcMain;

	if (type_ == GameWindowType::kSubWindow) {
		wc.lpfnWndProc = WindowProcSub;
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

void GameWindow::Term() {
	CloseWindow(hwnd_);

	if (hInst_ != nullptr) {
		UnregisterClass(className_.c_str(), hInst_);
	}
}

LRESULT GameWindow::WindowProcMain(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	/*if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}*/

	switch (msg) {
		case WM_DESTROY: //!< ウィンドウが破棄された
			// OSに対して, アプリの終了を伝える
			PostQuitMessage(0);
			return 0;
			break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

LRESULT GameWindow::WindowProcSub(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {
		case WM_CLOSE: //!< ウィンドウが破棄された
			// OSに対して, アプリの終了を伝える
			DestroyWindow(hwnd);
			return 0;
			break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

const GameWindow* GameWindowCollection::CreateMainWindow(const Vector2ui& clientSize, const LPCWSTR name) {
	Assert(mainWindow_ == nullptr, "main window has already been created.");
	//!< main window がすでに作られている

	mainWindow_ = std::make_unique<GameWindow>();
	mainWindow_->Create(clientSize, name);

	return mainWindow_.get();
}

const std::weak_ptr<GameWindow> GameWindowCollection::TryCreateSubWindow(const Vector2ui& clientSize, const LPCWSTR name) {

	if (!subWindows_.contains(name)) {
		//!< windowが作られていない場合, 生成
		std::unique_ptr<GameWindow> window = std::make_unique<GameWindow>();
		window->Create(clientSize, name, mainWindow_->GetHwnd());

		subWindows_.emplace(name, std::move(window));
	}

	return subWindows_.at(name);
}

void GameWindowCollection::Term() {
	subWindows_.clear();
	mainWindow_.reset();
}

bool GameWindowCollection::ProcessMessages() {

	MSG msg = {};

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT) {
		return false;
	}

	RemoveClosedWindow();

	return true;
}

void GameWindowCollection::RemoveClosedWindow() {
	for (auto it = subWindows_.begin(); it != subWindows_.end();) {

		if (!(*it).second->IsOpenWindow()) {
			it = subWindows_.erase(it);
			continue;
		}

		it++;
	}
}
