#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* window
#include "GameWindow.h"

//* lib
#include <Lib/Geometry/Vector4.h>

//* c++
#include <memory>
#include <unordered_map>

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

	GameWindow* CreateMainWindow(
		const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor = kDefaultGameWindowColor
	);

	const std::weak_ptr<GameWindow> TryCreateSubWindow(
		const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor = kDefaultGameWindowColor
	);

	void Term();

	//! @brief ProcessMessageの取得
	//! @retval true  ループ継続
	//! @retval false 終了
	bool ProcessMessage();

	void PresentAllWindow();

	//* getter *//

	GameWindow* GetMainWindow() const { return mainWindow_.get(); }

	const std::weak_ptr<GameWindow> TryGetSubWindow(const LPCWSTR& name) const noexcept;
	const std::weak_ptr<GameWindow> GetSubWindow(const LPCWSTR& name) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* windows *//

	std::unique_ptr<GameWindow>                              mainWindow_;
	std::unordered_map<LPCWSTR, std::shared_ptr<GameWindow>> subWindows_;

	// CONSIDER: dequeシステムで一番先頭がmain, その他がsubとしたい
	//+ debug, releaseでのmainの設定が簡単
	//- windowの検索がo(n)になる
	//- 型をそろえる必要がある

	//=========================================================================================
	// private methods
	//=========================================================================================

	void RemoveClosedSubWindow();

};