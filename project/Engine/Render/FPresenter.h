#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FPresenter class
////////////////////////////////////////////////////////////////////////////////////////////
class FPresenter {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Present(const DirectXQueueContext* context, const Vector2ui& windowSize, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

};
