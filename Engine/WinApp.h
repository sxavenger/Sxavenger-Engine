#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// windows
#include <windows.h>

// c++
#include <cstdint>
#include <string>
#include <cassert>

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
	void CreateGameWindow(int32_t clientWidth = 1280, int32_t clientHeight = 720, const wchar_t* windowTitle = L"DirectXGame");

	//! @brief 終了処理
	void Term();

	//! @breif プロセスメッセージを取得
	//! 
	//! @retval ture  終了メッセージ
	//! @retval false 継続
	bool ProcessMessage();

	//! @breif インスタンスを取得
	//! 
	//! @return インスタンスを返却
	static WinApp* GetInstance();

	const HWND& GetHwnd() const { return hWnd_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	const wchar_t* kWindowClassName = L"DirectXGame";

	HINSTANCE hInst_;
	HWND      hWnd_;
	int32_t   clientWidth_;
	int32_t   clientHeight_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};