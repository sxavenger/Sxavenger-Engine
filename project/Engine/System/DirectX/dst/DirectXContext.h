#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObject/DxCommandContext.h"
#include "DxObject/DxSwapChain.h"
#include "DxObject/DxDepthStencil.h"

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Color4.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXWindowContext class
////////////////////////////////////////////////////////////////////////////////////////////
class DirectXWindowContext {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DirectXWindowContext()  = default;
	~DirectXWindowContext() { Term(); }

	void Init(Window* window, DirectXThreadContext* context, const Color4f& clearColor);

	void Term();

	void BeginRendering();

	void EndRendering();

	void ClearWindow(bool isClearColor = true, bool isClearDepth = true);

	void TryClearWindow(bool isClearColor = true, bool isClearDepth = true);

	void Present(bool isTransitonAllocator = true);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* extrenal *//

	DirectXThreadContext* threadContext_ = nullptr;

	//* DXOBJECT *//

	std::unique_ptr<DxObject::SwapChain>    swapChain_;
	std::unique_ptr<DxObject::DepthStencil> depthStencil_;

	//* paraemeter *//

	Color4f clearColor_ = {};

	bool isClearWindow_ = false;

	DXGI_COLOR_SPACE_TYPE colorSpace_ = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;
	float minLuminance_ = 0.0f;
	float maxLuminance_ = 0.0f;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CheckSupportHDR(const HWND& hwnd);

	static int32_t ComputeIntersectionArea(
		const Vector2i& leftTopA, const Vector2i& rightBottomA,
		const Vector2i& leftTopB, const Vector2i& rightBottomB
	);

};
