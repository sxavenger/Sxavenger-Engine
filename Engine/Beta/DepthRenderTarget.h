#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DxObject/DxDescriptorHeaps.h>
#include <Engine/System/DirectXCommon.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DepthRenderTarget class
////////////////////////////////////////////////////////////////////////////////////////////
class DepthRenderTarget {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DepthRenderTarget() = default;

	~DepthRenderTarget() { Term(); }

	void Create(
		const Vector2ui& size, const Color4f& clearColor = defaultClearColor, DXGI_FORMAT format = DxObject::forwardFormat
	);

	void Term();

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleRTV() const { return descriptorRTV_.GetCPUHandle(); }
	
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleDSV() const { return descriptorDSV_.GetCPUHandle(); }

	const Vector2ui& GetSize() const { return size_; }

	const Color4f& GetClearColor() const { return clearColor_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	DxObject::DescriptorHeaps* descriptorHeaps_ = nullptr;

	//* RenderTarget *//

	ComPtr<ID3D12Resource> resource_;
	DxObject::Descriptor descriptorRTV_;
	DxObject::Descriptor descriptorSRV_;

	//* DepthStencil *//

	ComPtr<ID3D12Resource> resourceDSV_;
	DxObject::Descriptor descriptorDSV_;

	//* paraeter *//

	Vector2ui size_;
	Color4f clearColor_;
	DXGI_FORMAT format_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateRenderTarget();
	void CreateDepthStencil();

};