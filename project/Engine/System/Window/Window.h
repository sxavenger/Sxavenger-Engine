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
#include <filesystem>

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
	~Window() { Close(); }

	void Create(const Vector2ui& clientSize, const LPCWSTR name, const HWND parentHwnd = nullptr);
	void CreateEx(const Vector2ui& clientSize, const LPCWSTR name, const WNDPROC& proc, const HWND parentHwnd = nullptr);

	void Close();

	void SetIcon(const std::filesystem::path& filepath, const Vector2ui& cursolSize);
	void SetWindowIcon(const std::filesystem::path& filepath, const Vector2ui& cursolSize);
	void SetTaskbarIcon(const std::filesystem::path& filepath, const Vector2ui& cursolSize);

	//* getter *//

	const HINSTANCE& GetHInst() const { return hInst_; }

	const HWND& GetHwnd() const { return hwnd_; }

	bool IsOpenWindow() const { return hwnd_ != nullptr && IsWindow(hwnd_); }

	const Vector2ui& GetSize() const { return clientSize_; }

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
