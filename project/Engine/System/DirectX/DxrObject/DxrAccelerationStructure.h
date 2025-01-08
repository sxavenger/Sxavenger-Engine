#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"

//* DXOBJECT
#include <Engine/System/DirectX/DxObject/DxDevice.h>
#include <Engine/System/DirectX/DxObject/DxCommandContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// AccelerationStructureBuffers structure
////////////////////////////////////////////////////////////////////////////////////////////
struct AccelerationStructureBuffers {

	//* member *//

	ComPtr<ID3D12Resource> scratch;
	ComPtr<ID3D12Resource> asbuffer;
	ComPtr<ID3D12Resource> update;

	//* methods *//

	void Create(
		DxObject::Device* device,
		const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs
	);

};

////////////////////////////////////////////////////////////////////////////////////////////
// BottomLevelAS class
////////////////////////////////////////////////////////////////////////////////////////////
class BottomLevelAS {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BottomLevelAS()  = default;
	~BottomLevelAS() = default;



private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	AccelerationStructureBuffers buffers_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Build(
		DxObject::Device* device, const DxObject::CommandContext* context,
		const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc
	);

};

_DXROBJECT_NAMESPACE_END
