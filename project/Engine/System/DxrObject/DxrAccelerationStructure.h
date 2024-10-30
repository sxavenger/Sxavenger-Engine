#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include <Engine/System/DxrObject/DxrObjectCommon.h>
#include <Engine/System/DxrObject/DxrBufferRecorder.h>

//* DXOBEJCT
#include <Engine/System/DxObject/DxBufferResource.h>

//* engine
#include <Engine/Game/InputAssembler.h>

//* lib
#include <Lib/ObjectStructure.h>

//* c++
#include <vector>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// AccelerationStructureBuffers structure
////////////////////////////////////////////////////////////////////////////////////////////
struct AccelerationStructureBuffers {
	ComPtr<ID3D12Resource> scratch;
	ComPtr<ID3D12Resource> asbuffer;
	ComPtr<ID3D12Resource> update;
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
	~BottomLevelAS() { Term(); }

	void Create(const InputAssembler<VertexData>& ia);

	void Term();

	//* getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return buffers_.asbuffer->GetGPUVirtualAddress(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* buffers *//

	AccelerationStructureBuffers buffers_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void BuildBLAS(const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc);

};

////////////////////////////////////////////////////////////////////////////////////////////
// TopLevelAS class
////////////////////////////////////////////////////////////////////////////////////////////
class TopLevelAS {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// InstanceElements structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct InstanceElements {
		const BottomLevelAS*   blas;
		Matrix4x4              worldMatrix;
		UINT                   instanceId;
		const BufferRecoreder* recorder;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TopLevelAS()  = default;
	~TopLevelAS() { Term(); }

	void Create();

	void Term();

	//* instance option *//

	void BeginSetupInstance();

	void EndSetupInstance();

	void SetInstance(
		const BottomLevelAS* blas, const Matrix4x4& worldMatrix, const BufferRecoreder* recorder, UINT instanceId = 0
	);

	//* getter *//

	const std::vector<InstanceElements>& GetInstances() const { return instances_; }

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return buffers_.asbuffer->GetGPUVirtualAddress(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* buffers *//

	AccelerationStructureBuffers buffers_;

	//* instanceBuffer *//

	static const uint32_t kMaxInstanceNum_ = 256;

	std::vector<InstanceElements>                                             instances_;
	std::unique_ptr<DxObject::BufferResource<D3D12_RAYTRACING_INSTANCE_DESC>> instanceBuffer_;

	uint32_t preInstanceCount_ = 0;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateInstance();
	void SetInstanceBuffer();

	void BuildTLAS();
	void UpdateTLAS();
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

AccelerationStructureBuffers CreateAccelerationStructure(
	const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs
);

_DXROBJECT_NAMESPACE_END