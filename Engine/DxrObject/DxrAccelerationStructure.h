#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxrObjectCommon
#include <DxrObjectCommon.h>

//* DxrObject
#include <DxrExportGroup.h>
#include <DxrBufferRecord.h>

//* DxObject
#include <DxBufferResource.h>

//* 3DObjects
#include <InputAssembler.h>
#include <ObjectStructure.h>

//* c++
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject
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

	BottomLevelAS() = default;

	//! @brief InputAssemblerからBLASの構築
	void Create(const InputAssembler<VertexData>& ia, const ExportGroup* exportGroup);

	// todo: AABBからの構築

	//* recorder option *//

	void BindAddress(uint32_t index, const D3D12_GPU_VIRTUAL_ADDRESS address);
	void BindHandle(uint32_t index, const D3D12_GPU_DESCRIPTOR_HANDLE handle);

	const BufferRecord* GetRecoreder() const { return recorder_.get(); }

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return buffers_.asbuffer->GetGPUVirtualAddress(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* buffers *//

	AccelerationStructureBuffers buffers_;

	//* recorder *//

	std::unique_ptr<BufferRecord> recorder_;
	
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
		BottomLevelAS* blas;
		Matrix4x4      worldMatrix;
		UINT           instanceId;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TopLevelAS() { Init(); }

	~TopLevelAS() { Term(); }

	void Init();

	void Term();

	//* instance option *//

	void BeginSetupInstance();
	void EndSetupInstance();

	void SetInstance(BottomLevelAS* blas, const Matrix4x4& worldMatrix, UINT instanceId);

	//* getter *//

	const std::list<InstanceElements>& GetInstance() const { return instance_; }

	const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return descriptor_.GetGPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* buffers *//

	AccelerationStructureBuffers buffers_;

	//* instanceBuffer *//

	std::list<InstanceElements>                                               instance_;
	std::unique_ptr<DxObject::BufferResource<D3D12_RAYTRACING_INSTANCE_DESC>> instanceBuffer_;
	DxObject::Descriptor descriptor_;

	static const uint32_t kMaxInstanceNum_ = 256;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void BuildTLAS();
	void UpdateTLAS();

	void SetInstanceBuffer();

};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

AccelerationStructureBuffers CreateAccelerationStructure(
	const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs
);

ComPtr<ID3D12Resource> CreateBuffer(
	D3D12_HEAP_TYPE heapType,
	UINT64 bufferSize,
	D3D12_RESOURCE_FLAGS flags,
	D3D12_RESOURCE_STATES initState
);

_DXROBJECT_NAMESPACE_END