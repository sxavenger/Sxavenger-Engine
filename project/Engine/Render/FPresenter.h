#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h>
#include <Engine/System/DirectX/DirectXContext.h>

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

	FPresenter()  = default;
	~FPresenter() = default;

	void Init();

	void Present(const DirectXThreadContext* context, const Vector2ui& windowSize, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> pipeline_;

};
