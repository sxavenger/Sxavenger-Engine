#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#define NOMINMAX
#include <windows.h>

//* c++
#include <cstdint>

//* geometry
#include <Lib/Geometry/Vector2.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "winmm.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// WinApp class
////////////////////////////////////////////////////////////////////////////////////////////
class WinApp {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	WinApp();
	~WinApp();

	//! @breif ウィンドウを作成
	//! 
	//! @param[in] clientWidth  クライアント領域の横幅
	//! @param[in] clientHeight クライアント領域の縦幅
	//! @param[in] windowTitle  ウィンドウタイトル
	void CreateGameWindow(const Vector2ui& clientSize = { 1280, 720 }, const wchar_t* windowTitle = L"Sxavenger Engine");

	//! @brief 終了処理
	void Term();

	//! @breif プロセスメッセージを取得
	//! 
	//! @retval ture  終了メッセージ
	//! @retval false 継続
	bool ProcessMessage();

	const HWND& GetHwnd() const { return hWnd_; }

	const HINSTANCE& GetHinst() const { return hInst_; }

	const Vector2ui& GetClientSize() const { return clientSize_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	const wchar_t* kWindowClassName_ = L"Sxavenger Engine";

	HINSTANCE hInst_;
	HWND      hWnd_;
	Vector2ui clientSize_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};