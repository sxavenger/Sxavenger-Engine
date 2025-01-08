#include "DxrAccelerationStructure.h"
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// AccelerationStructureBuffers structure
////////////////////////////////////////////////////////////////////////////////////////////

void AccelerationStructureBuffers::Create(
	DxObject::Device* devices,
	const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs) {

	auto device = devices->GetDevice();

	// 必要なメモリ数を求める
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
	device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

	// buffer生成
	scratch = DxObject::CreateBufferResource(
		device,
		D3D12_HEAP_TYPE_DEFAULT,
		info.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON
	);

	asbuffer = DxObject::CreateBufferResource(
		device,
		D3D12_HEAP_TYPE_DEFAULT,
		info.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE
	);

	if (inputs.Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE) {
		update = DxObject::CreateBufferResource(
			device,
			D3D12_HEAP_TYPE_DEFAULT,
			info.UpdateScratchDataSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_COMMON
		);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////
// BottomLevelAS class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BottomLevelAS::Build(
	DxObject::Device* device, const DxObject::CommandContext* context,
	const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc) {

	// build descの設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
	buildDesc.Inputs.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	buildDesc.Inputs.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
	buildDesc.Inputs.Flags          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
	buildDesc.Inputs.NumDescs       = 1;
	buildDesc.Inputs.pGeometryDescs = &geomDesc;

	// input情報からbufferの生成
	buffers_.Create(device, buildDesc.Inputs);

	// bufferの設定
	buildDesc.ScratchAccelerationStructureData = buffers_.scratch->GetGPUVirtualAddress();
	buildDesc.DestAccelerationStructureData    = buffers_.asbuffer->GetGPUVirtualAddress();

	// build descの設定
	context->GetCommandList()->BuildRaytracingAccelerationStructure(
		&buildDesc, 0, nullptr
	);

	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = buffers_.asbuffer.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}
