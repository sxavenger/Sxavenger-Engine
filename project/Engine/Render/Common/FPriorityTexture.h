#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxObjectCommon.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FPriorityTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FPriorityTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FPriorityTexture() = default;
	~FPriorityTexture() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

	void ClearDepth(const DirectXQueueContext* context) const;

	//* getter *//

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleDSV() const { return descriptorDSV_.GetCPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ComPtr<ID3D12Resource> resource_;
	DxObject::Descriptor descriptorDSV_;

};
