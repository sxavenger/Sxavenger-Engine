#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Content/Texture/DepthTexture.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DepthBufferController class
////////////////////////////////////////////////////////////////////////////////////////////
class DepthBufferController {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DepthBufferController()  = default;
	~DepthBufferController() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

	//* depth buffer option *//

	void TransitionBeginRaytracingDepthWrite(const DirectXThreadContext* context);

	void TransitionEndRaytracingDepthWrite(const DirectXThreadContext* context);

	void TransferRaytracingToRasterize(const DirectXThreadContext* context);
	//* rasterize, raytracing どちらも depthWrite状態じゃないことが条件

	void TransitionBeginRasterizeDepthWrite(const DirectXThreadContext* context);

	void TransitionEndRasterizeDepthWrite(const DirectXThreadContext* context);

	void ClearRasterizeDepth(const DirectXThreadContext* context);

	//* getter *//

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRasterizerCPUHandleDSV() const { return resterizerDepth_->GetCPUHandleDSV(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* rasterizer depth *//

	std::unique_ptr<DepthTexture> resterizerDepth_;

	//* raytracing depth *//

	ComPtr<ID3D12Resource> raytracingDepthBuffer_;
	DxObject::Descriptor descriptorUAV_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateRaytracingDepth(const Vector2ui& size);

};