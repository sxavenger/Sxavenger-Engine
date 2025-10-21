#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/Context/DirectXWindowContext.h>

//* c++
#include <memory>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// WindowCollection class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Windowを管理するクラス.
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

	bool ProcessMessage();

	void PresentWindows();

	DirectXWindowContext* GetMainWindow() const { return main_.get(); }

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
