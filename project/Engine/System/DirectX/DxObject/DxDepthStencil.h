#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxDescriptor.h"
#include "DxDescriptorHeaps.h"

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// DepthSteincil class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 深度ステンシルバッファを管理するクラス.
class DepthStencil {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DepthStencil()  = default;
	~DepthStencil() { Term(); }

	void Init(
		Device* device, DescriptorHeaps* descriptorHeaps,
		const Vector2ui& size, DXGI_FORMAT format = DxObject::kDefaultDepthFormat
	);

	void Term();

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle() const { return descriptorDSV_.GetCPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* buffer *//

	ComPtr<ID3D12Resource> resource_;

	DxObject::Descriptor descriptorDSV_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateResource(Device* device, const Vector2ui& size, DXGI_FORMAT format);
	void CreateDSV(Device* devices, DescriptorHeaps* descriptorHeaps, DXGI_FORMAT format);

};

DXOBJECT_NAMESPACE_END
