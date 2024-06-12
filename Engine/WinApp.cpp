#include "WinApp.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

////////////////////////////////////////////////////////////////////////////////////////////
// WinApp methods
////////////////////////////////////////////////////////////////////////////////////////////

WinApp::WinApp() :
	hInst_(nullptr),
	hWnd_(nullptr) {

}

WinApp::~WinApp() {
}

void WinApp::CreateGameWindow(int32_t clientWidth, int32_t clientHeight, const wchar_t* windowTitle) {
	// インスタンスハンドルを取得
	auto hInst = GetModuleHandle(nullptr);
	assert(hInst != nullptr);

	hInst_ = hInst;

	// ウィンドウの設定
	WNDCLASS wc = {};
	wc.lpfnWndProc   = WindowProc;
	wc.lpszClassName = kWindowClassName;
	wc.hInstance     = hInst_;
	wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);

	if (!RegisterClass(&wc)) {
		assert(false);
	}

	// ウィンドウサイズの設定
	clientWidth_ = clientWidth;
	clientHeight_ = clientHeight;

	RECT rc = {};
	rc.right  = clientWidth_;
	rc.bottom = clientHeight_;

	// ウィンドウサイズの調整
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウを生成
	hWnd_ = CreateWindow(
		wc.lpszClassName,
		windowTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, //!< windowのサイズの固定
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInst_,
		nullptr
	);

	if (hWnd_ == nullptr) {
		assert(false);
	}

	// ウィンドウを表示
	ShowWindow(hWnd_, SW_SHOW);
}

void WinApp::Term() {
	CloseWindow(hWnd_);

	if (hInst_ != nullptr) {
		UnregisterClass(kWindowClassName, hInst_);
	}

	hInst_ = nullptr;
	hWnd_  = nullptr;
}

bool WinApp::ProcessMessage() {
	MSG msg = {};

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT) {
		return true;
	}

	return false;
}

WinApp* WinApp::GetInstance() {
	static WinApp instance;
	return &instance;
}

LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
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
