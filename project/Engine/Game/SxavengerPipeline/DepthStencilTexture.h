#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DxObject/DxDescriptorHeaps.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DepthStencilTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class DepthStencilTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DepthStencilTexture() = default;
	~DepthStencilTexture() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

	//* option *//

	void BeginDepthWrite(bool isClearDepth = false);

	void EndDepthWrite();

	//* getter *//

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleDSV() const { return descriptorDSV_.GetCPUHandle(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	DxObject::DescriptorHeaps* descriptorHeaps_ = nullptr;

	//* resource *//

	ComPtr<ID3D12Resource> resource_;

	DxObject::Descriptor descriptorDSV_;
	DxObject::Descriptor descriptorSRV_;

	//* parameter *//

	Vector2ui size_;

	//=========================================================================================
	// private variables
	//=========================================================================================

	void CreateResource();
	void CreateDSV();
	void CreateSRV();

};