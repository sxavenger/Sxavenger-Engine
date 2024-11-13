#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#include <windows.h>

//* lib
#include <Lib/Geometry/Vector2.h>

//* c++
#include <optional>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// WindowType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class WindowType {
	kMainWindow,
	kSubWindow
};

////////////////////////////////////////////////////////////////////////////////////////////
// Window class
////////////////////////////////////////////////////////////////////////////////////////////
class Window {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Window()  = default;
	~Window() { Term(); }

	void Create(const Vector2ui& clientSize, const LPCWSTR name, const HWND parentHwnd = nullptr);

	void Term();

	//* getter *//

	const HINSTANCE& GetHInst() const { return hInst_; }

	const HWND& GetHwnd() const { return hwnd_; }

	bool IsOpenWindow() const { return hwnd_ != nullptr && IsWindow(hwnd_); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* window info *//

	std::optional<WindowType> type_ = std::nullopt;

	//* window parameter *//

	HINSTANCE hInst_;
	HWND      hwnd_;

	Vector2ui clientSize_;
	LPCWSTR   name_;

	std::wstring className_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* window proc *//

	static LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK SubWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

};