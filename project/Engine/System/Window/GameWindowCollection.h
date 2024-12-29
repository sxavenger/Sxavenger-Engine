#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* window
#include "GameWindow.h"

//* engine
#include <Engine/System/UI/ISystemDebugGui.h>

//* lib
#include <Lib/Geometry/Vector4.h>
#include <Lib/Sxl/OptimizedMap.h>

//* c++
#include <memory>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// GameWindowCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class GameWindowCollection
	: public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GameWindowCollection()  = default;
	~GameWindowCollection() { Term(); }

	const std::weak_ptr<GameWindow> CreateMainWindow(
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

	const std::weak_ptr<GameWindow> GetMainWindow() const;

	const std::weak_ptr<GameWindow> TryGetSubWindow(const LPCWSTR& name) const noexcept;
	const std::weak_ptr<GameWindow> GetSubWindow(const LPCWSTR& name) const;

	const GameWindow* GetForcusWindow() const;

	void SystemDebugGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* windows *//

	std::shared_ptr<GameWindow>                             mainWindow_;
	Sxl::OptimizedMap<LPCWSTR, std::shared_ptr<GameWindow>> subWindows_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void RemoveClosedSubWindow();

};
