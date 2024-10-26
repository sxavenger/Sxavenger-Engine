#include "DxrAccelerationStructure.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BottomLevelAS class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BottomLevelAS::Create(const InputAssembler<VertexData>& ia) {

	// GeometryDescの生成
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = {};
	geomDesc.Type                                 = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geomDesc.Flags                                = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	geomDesc.Triangles.VertexBuffer.StartAddress  = ia.GetVertexBuffer()->GetGPUVirtualAddress();
	geomDesc.Triangles.VertexBuffer.StrideInBytes = ia.GetVertexBuffer()->GetStructureSize();
	geomDesc.Triangles.VertexCount                = ia.GetVertexBuffer()->GetIndexSize();
	geomDesc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT;
	geomDesc.Triangles.IndexBuffer                = ia.GetIndexBuffer()->GetGPUVirtualAddress();
	geomDesc.Triangles.IndexCount                 = ia.GetIndexBuffer()->GetIndexSize();
	geomDesc.Triangles.IndexFormat                = DXGI_FORMAT_R32_UINT;

	BuildBLAS(geomDesc);
}

void BottomLevelAS::Term() {
}

void BottomLevelAS::BuildBLAS(const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc) {
	
	//* buildASDesc *//
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
	buildASDesc.Inputs.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	buildASDesc.Inputs.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
	buildASDesc.Inputs.Flags          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
	buildASDesc.Inputs.NumDescs       = 1;
	buildASDesc.Inputs.pGeometryDescs = &geomDesc;

	// input情報からbufferの生成
	buffers_ = CreateAccelerationStructure(
		buildASDesc.Inputs
	);

	// bufferの設定
	buildASDesc.ScratchAccelerationStructureData = buffers_.scratch->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData    = buffers_.asbuffer->GetGPUVirtualAddress();

	// commandListの取得
	auto commandList = Sxavenger::GetCommandList();

	// Build
	commandList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr
	);

	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags         = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.UAV.pResource = buffers_.asbuffer.Get();

	commandList->ResourceBarrier(1, &barrier);
}

////////////////////////////////////////////////////////////////////////////////////////////
// TopLevelAS class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TopLevelAS::Create() {

	CreateInstance();

	// 更新
	BuildTLAS();
}

void TopLevelAS::Term() {
}

void TopLevelAS::BeginSetupInstance() {
	instances_.clear(); // 新しく設定するのでclearしておく
}

void TopLevelAS::EndSetupInstance() {
	// bufferの更新
	SetInstanceBuffer();

	// 更新
	BuildTLAS();
}

void TopLevelAS::SetInstance(const BottomLevelAS* blas, const Matrix4x4& worldMatrix, const BufferRecoreder* recorder, UINT instanceId) {
	instances_.emplace_back(blas, worldMatrix, instanceId, recorder);
}

void TopLevelAS::CreateInstance() {

	// instance bufferの生成
	instanceBuffer_
		= std::make_unique<DxObject::BufferResource<D3D12_RAYTRACING_INSTANCE_DESC>>(Sxavenger::GetDevicesObj(), kMaxInstanceNum_);

	instances_.reserve(kMaxInstanceNum_);
}

void TopLevelAS::SetInstanceBuffer() {

	// 前フレームの分のBuffer情報をclear
	instanceBuffer_->ClearData();

	for (uint32_t i = 0; i < instances_.size(); ++i) {
		Assert(i < kMaxInstanceNum_); //!< instance数の超過

		// bufferにinstance情報を設定
		auto& desc = (*instanceBuffer_)[i];
		desc.Flags        = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		desc.InstanceMask = 0xFF;

		// matrixを3x4に変更
		Matrix4x4 mat = instances_[i].worldMatrix.Transpose();
		std::memcpy(desc.Transform, &mat, sizeof(desc.Transform));

		desc.AccelerationStructure               = instances_[i].blas->GetGPUVirtualAddress();
		desc.InstanceID                          = instances_[i].instanceId;
		desc.InstanceContributionToHitGroupIndex = i; //!< hack
	}
}

void TopLevelAS::BuildTLAS() {

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
	buildASDesc.Inputs.Type          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	buildASDesc.Inputs.DescsLayout   = D3D12_ELEMENTS_LAYOUT_ARRAY;
	buildASDesc.Inputs.Flags         = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	buildASDesc.Inputs.NumDescs      = kMaxInstanceNum_;
	buildASDesc.Inputs.InstanceDescs = instanceBuffer_->GetGPUVirtualAddress();

	buffers_ = CreateAccelerationStructure(
		buildASDesc.Inputs
	);

	buildASDesc.ScratchAccelerationStructureData = buffers_.scratch->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData    = buffers_.asbuffer->GetGPUVirtualAddress();

	// コマンドに積む
	auto commandList = Sxavenger::GetCommandList();

	commandList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr
	);

	// バリアの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags         = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.UAV.pResource = buffers_.asbuffer.Get();

	commandList->ResourceBarrier(1, &barrier);

}

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

AccelerationStructureBuffers DxrObject::CreateAccelerationStructure(
	const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs) {

	AccelerationStructureBuffers result = {};

	// deviceの取り出し
	auto device = Sxavenger::GetDevice();

	// 必要なメモリ数を求める
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
	device->GetRaytracingAccelerationStructurePrebuildInfo(
		&inputs, &info
	);

	// bufferの生成
	result.scratch = DxObject::CreateBufferResource(
		device,
		D3D12_HEAP_TYPE_DEFAULT,
		info.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON
	);

	result.asbuffer = DxObject::CreateBufferResource(
		device,
		D3D12_HEAP_TYPE_DEFAULT,
		info.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE
	);

	// アップデート用Bufferの生成
	if (inputs.Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE) {
		result.update = DxObject::CreateBufferResource(
			device,
			D3D12_HEAP_TYPE_DEFAULT,
			info.UpdateScratchDataSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_COMMON
		);
	}

	return result;
}
