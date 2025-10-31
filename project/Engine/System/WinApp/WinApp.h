#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#include <windows.h>

//* mf
#include <mfapi.h>

//* c++
#include <filesystem>
#include <optional>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
//* winmm
#pragma comment(lib, "winmm.lib")

//* mf
#pragma comment(lib, "mfplat.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// WinApp class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Windowsアプリケーションの管理クラス.
class WinApp {
public:

	//=========================================================================================
	// public methods
	//========================================================================================

	static void Init();

	static void Term();

	//* windows api option *//

	//! @brief Dialogを表示して保存するファイル名を取得する.
	//! @param title   Dialogのタイトル
	//! @param current Dialogの初期表示ファイル名
	//! @param filter Dialogのフィルタ設定 {表示名, 拡張子}
	//! @param extension 何も指定されなかった場合に付与する拡張子
	//! @retval std::filesystem::path 保存するファイル名
	//! @retval std::nullopt          キャンセルされた場合
	static std::optional<std::filesystem::path> GetSaveFilepath(
		const std::wstring& title,
		const std::filesystem::path& current,
		const std::pair<std::wstring, std::wstring>& filter,
		const std::filesystem::path& extension
	);

	//! @brief Dialogを表示して開くファイル名を取得する.
	//! @param title  Dialogのタイトル
	//! @param current Dialogの初期表示ファイル名
	//! @param filter Dialogのフィルタ設定 {表示名, 拡張子}
	//! @retval std::filesystem::path 開くファイル名
	//! @retval std::nullopt          キャンセルされた場合
	static std::optional<std::filesystem::path> GetOpenFilepath(
		const std::wstring& title,
		const std::filesystem::path& current,
		const std::pair<std::wstring, std::wstring>& filter
	);

private:
};
