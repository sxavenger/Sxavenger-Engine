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
// DepthTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class DepthTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DepthTexture()  = default;
	~DepthTexture() { Term(); }

	void Create(const Vector2ui& size); //!< default depth format only.

	void Term();

	//* depth option *//

	void TransitionBeginDepthWrite(const DirectXThreadContext* context);

	void TransitionEndDepthWrite(const DirectXThreadContext* context);

	void ClearDepth(const DirectXThreadContext* context);

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleDSV() const { return descriptorDSV_.GetCPUHandle(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12Resource> resource_;

	DxObject::Descriptor descriptorDSV_;
	DxObject::Descriptor descriptorSRV_;

	//* paraemter *//

	Vector2ui size_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateResource();
	void CreateDSV();
	void CreateSRV();

};