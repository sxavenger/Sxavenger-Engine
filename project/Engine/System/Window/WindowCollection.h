#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXWindowContext.h>
#include <Engine/System/UI/ISystemDebugGui.h>

//* c++
#include <memory>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// WindowCollection class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief windowを管理するクラス.
class WindowCollection
	: public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	std::weak_ptr<DirectXWindowContext> CreateMainWindow(const Vector2ui& size, const std::wstring& name, const Color4f& color = DirectXWindowContext::kDefaultClearColor);

	std::weak_ptr<DirectXWindowContext> CreateSubWindow(const Vector2ui& size, const std::wstring& name, DirectXWindowContext::ProcessCategory category, const Color4f& color = DirectXWindowContext::kDefaultClearColor);

	void Term();

	//* window option *//

	//! @brief メッセージ処理
	//! @retval true  メッセージが存在する
	//! @retval false windowが閉じられた
	bool ProcessMessage();

	//! @brief 全てのwindowのPresent実行.
	void PresentWindows();

	//! @brief メインウィンドウの取得
	//! @return windowのptr
	DirectXWindowContext* GetMainWindow() const { return main_.get(); }

	//! @brief フォーカスされているウィンドウの取得
	//! @retval ptr フォーカスされているwindowのptr
	//! @retval nullptr windowが存在しない
	DirectXWindowContext* GetForcusWindow() const;

	//* debug option *//

	void SystemDebugGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* windows *//

	std::shared_ptr<DirectXWindowContext> main_;

	std::unordered_map<std::wstring, std::shared_ptr<DirectXWindowContext>> windows_;

	std::unordered_map<HWND, DirectXWindowContext*> hwnds_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void RemoveClosedWindow();

};

SXAVENGER_ENGINE_NAMESPACE_END
