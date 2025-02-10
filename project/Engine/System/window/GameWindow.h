#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/System/Window/Window.h>

//* engine
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Color.h>

////////////////////////////////////////////////////////////////////////////////////////////
// static variables
////////////////////////////////////////////////////////////////////////////////////////////

static const Color4f kDefaultGameWindowColor = ToColor4<float>(0x9BA8A8FF);

////////////////////////////////////////////////////////////////////////////////////////////
// GameWindow class
////////////////////////////////////////////////////////////////////////////////////////////
class GameWindow
	: public Window, public DirectXWindowContext {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(
		const Vector2ui& clientSize, const LPCWSTR& name,
		DirectXThreadContext* threadContext,
		const HWND& parentHwnd = nullptr, const Color4f& clearColor = kDefaultGameWindowColor
	);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

};
