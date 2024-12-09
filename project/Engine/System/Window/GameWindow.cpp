#include "GameWindow.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameWindow class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameWindow::Create(const Vector2ui& clientSize, const LPCWSTR& name, DirectXThreadContext* threadContext, const HWND& parentHwnd, const Color4f& clearColor) {
	Window::Create(clientSize, name, parentHwnd);                //!< window自体の生成
	DirectXWindowContext::Init(this, threadContext, clearColor); //!< windowに関与するDirectX12の初期化
}
