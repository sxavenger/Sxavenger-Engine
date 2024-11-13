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
#include <memory>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// GameWindowType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class GameWindowType {
	kMainWindow,
	kSubWindow
};

////////////////////////////////////////////////////////////////////////////////////////////
// GameWindow class
////////////////////////////////////////////////////////////////////////////////////////////
class GameWindow {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GameWindow() = default;
	~GameWindow() { Term(); }

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

	std::optional<GameWindowType> type_ = std::nullopt;

	//* window parameter *//

	HINSTANCE hInst_;
	HWND      hwnd_;

	Vector2ui clientSize_;
	LPCWSTR   name_;

	std::wstring className_;

	//* icon parameter *//

	LPCSTR windowIconFilePath_ = "";

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* window process *//

	static LRESULT CALLBACK WindowProcMain(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK WindowProcSub(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

};

////////////////////////////////////////////////////////////////////////////////////////////
// GameWindowCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class GameWindowCollection {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GameWindowCollection()  = default;
	~GameWindowCollection() { Term(); }

	const GameWindow* CreateMainWindow(const Vector2ui& clientSize, const LPCWSTR name);

	const std::weak_ptr<GameWindow> TryCreateSubWindow(const Vector2ui& clientSize, const LPCWSTR name);

	void Term();

	//! @retval true  継続
	//! @retval false 終了
	bool ProcessMessages();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<GameWindow>                              mainWindow_;
	std::unordered_map<LPCWSTR, std::shared_ptr<GameWindow>> subWindows_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void RemoveClosedWindow();

};