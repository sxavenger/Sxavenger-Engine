#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"
#include "DxrBindBuffer.h"
#include "DxrExportGroup.h"

//* DXOBJECT
#include <Engine/System/DirectX/DxObject/DxDevice.h>
#include <Engine/System/DirectX/DxObject/DxCommandContext.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Sxl/Flag.h>

//* c++
#include <vector>

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
class BottomLevelAS
	: protected AccelerationStructureBuffers {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BottomLevelAS()  = default;
	~BottomLevelAS() = default;

	void Build(
		DxObject::Device* device, const DxObject::CommandContext* context,
		const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc
	);

	//* getter *//

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return asbuffer->GetGPUVirtualAddress(); }

protected:

	//=========================================================================================
	// protected methods
	//=========================================================================================

};

////////////////////////////////////////////////////////////////////////////////////////////
// TopLevelAS class
////////////////////////////////////////////////////////////////////////////////////////////
class TopLevelAS
	: protected AccelerationStructureBuffers {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Instance structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Instance {
		//* desc書き込み
		const BottomLevelAS*                             bottomLevelAS;
		Matrix4x4                                        mat;
		UINT                                             instanceId;
		Sxl::Flag<D3D12_RAYTRACING_INSTANCE_FLAGS, UINT> flag;

		//* table書き込み
		const ExportGroup*  expt;
		WriteBindBufferDesc parameter;
		//!< DXR1.1では不使用
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TopLevelAS()  = default;
	~TopLevelAS() = default;

	//* instance option *//

	void BeginSetupInstance();

	void AddInstance(const Instance& instance);

	void EndSetupInstance(DxObject::Device* device, DxObject::CommandContext* context);

	//* getter *//

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return asbuffer->GetGPUVirtualAddress(); }

	const std::vector<Instance>& GetInstances() const { return instances_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::vector<Instance> instances_;

	std::unique_ptr<DxObject::DimensionBuffer<D3D12_RAYTRACING_INSTANCE_DESC>> descs_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Build(DxObject::Device* device, DxObject::CommandContext* context);
	void Update(DxObject::CommandContext* context);

	bool UpdateInstanceBuffer(DxObject::Device* device);

};

_DXROBJECT_NAMESPACE_END
