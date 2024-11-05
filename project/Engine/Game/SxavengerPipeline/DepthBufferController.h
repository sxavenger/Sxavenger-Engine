#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DxObject/DxDescriptorHeaps.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DepthBufferController class
////////////////////////////////////////////////////////////////////////////////////////////
class DepthBufferContoller {
public:

	//=========================================================================================
	// private variables
	//=========================================================================================

	DepthBufferContoller()  = default;
	~DepthBufferContoller() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

	//* depth buffer option *//

	void BeginRaytracingDepthWrite();

	void EndRaytracingDepthWrite();

	void TransferRaytracingToRasterize();
	//* rasterize, raytracing どちらも depthWrite状態じゃないことが条件

	void BeginRasterizeDepthWrite(bool isClearDepth = false);

	void EndRasterizeDepthWrite();

	//* getter *//

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRasterizeCPUHandleDSV() const { return descriptorDSV_.GetCPUHandle(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetRaytracingGPUHandleUAV() const { return descriptorUAV_.GetGPUHandle(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetRasterizeGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	DxObject::DescriptorHeaps* descriptorHeaps_ = nullptr;

	//* rasterize *//

	ComPtr<ID3D12Resource> rasterizeDepthBuffer_;

	DxObject::Descriptor descriptorDSV_;
	DxObject::Descriptor descriptorSRV_;

	//* raytracing *//

	ComPtr<ID3D12Resource> raytracingDepthBuffer_;

	DxObject::Descriptor descriptorUAV_;

	//* parameter *//

	Vector2ui size_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateRasterizeDepthBuffer();
	void CreateRaytracingDepthBuffer();

};