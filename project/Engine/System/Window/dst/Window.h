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
// Window class
////////////////////////////////////////////////////////////////////////////////////////////
class Window {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mode enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Mode : uint8_t {
		Borderless,
		Window
	};

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

	void SetWindowMode(Mode mode);

	//* getter *//

	const HINSTANCE& GetHInst() const { return hInst_; }

	const HWND& GetHwnd() const { return hwnd_; }

	bool IsOpenWindow() const { return hwnd_ != nullptr && IsWindow(hwnd_); }

	const Vector2ui& GetSize() const { return clientSize_; }

	Mode GetMode() const { return mode_; }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Category enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Category : bool {
		MainWindow,
		SubWindow
	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* window info *//

	std::optional<Category> type_ = std::nullopt;

	//* window parameter *//

	HINSTANCE hInst_;
	HWND      hwnd_;

	RECT rect_;

	Vector2ui clientSize_;
	LPCWSTR   name_;

	std::wstring className_;

	Mode mode_ = Mode::Window;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* window proc *//

	static LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK SubWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

};
