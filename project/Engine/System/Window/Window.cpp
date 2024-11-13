#include "Window.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Window class methods
////////////////////////////////////////////////////////////////////////////////////////////

LRESULT Window::MainWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

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

LRESULT Window::SubWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {
		case WM_CLOSE: //!< windowが終了した
			// windowの破棄
			DestroyWindow(hwnd);
			return 0;
			break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);

}
