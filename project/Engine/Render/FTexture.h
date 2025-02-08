#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DirectXContext.h>


//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FTexture()  = default;
	~FTexture() { Term(); }

	void Create(const Vector2ui& size, DXGI_FORMAT format = DxObject::kOffscreenFormat);

	void Term();

	//* transition methods *//

	D3D12_RESOURCE_BARRIER TransitionBeginRenderTarget() const;

	D3D12_RESOURCE_BARRIER TransitionEndRenderTarget() const;

	void ClearRenderTarget(const DirectXThreadContext* context) const;

	D3D12_RESOURCE_BARRIER TransitionBeginUnordered() const;

	D3D12_RESOURCE_BARRIER TransitionEndUnordered() const;

	D3D12_RESOURCE_BARRIER TransitionBeginState(D3D12_RESOURCE_STATES state) const;

	D3D12_RESOURCE_BARRIER TransitionEndState(D3D12_RESOURCE_STATES state) const;

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleRTV() const { return descriptorRTV_.GetCPUHandle(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV() const { return descriptorUAV_.GetGPUHandle(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

	const Vector2ui& GetSize() const { return size_; }

	const DXGI_FORMAT& GetFormat() const { return format_; }

	static const D3D12_RESOURCE_STATES GetDefaultState() { return kDefaultState_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* directx12 *//

	ComPtr<ID3D12Resource> resource_;

	DxObject::Descriptor descriptorUAV_;
	DxObject::Descriptor descriptorRTV_;
	DxObject::Descriptor descriptorSRV_;

	//* parameter *//

	Vector2ui   size_;
	DXGI_FORMAT format_;

	//* state *//

	static const D3D12_RESOURCE_STATES kDefaultState_ = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

};
