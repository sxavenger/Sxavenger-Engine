#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/System/Window/GameWindow.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector4.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// ScreenPresenter class
////////////////////////////////////////////////////////////////////////////////////////////
class ScreenPresenter {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ScreenPresenter()  = default;
	~ScreenPresenter() = default;

	void Init();

	void Term();

	void Present(GameWindow* window, const DirectXThreadContext* context, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ScreenPresentIA structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ScreenPresentIA {
		Vector4f position;
		Vector2f texcoord;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::VertexDimensionBuffer<ScreenPresentIA>> ia_;

	DxObject::BindBufferDesc bind_ = {};

};