#include "DxrAccelerationStructure.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BottomLevelAS class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BottomLevelAS::Create(const InputAssembler<VertexData>& ia, const ExportGroup* exportGroup) {

	//* GeometryDesc *//
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = {};
	geomDesc.Type  = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geomDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	//* triangle
	geomDesc.Triangles.VertexBuffer.StartAddress  = ia.GetVertexBuffer()->GetGPUVirtualAddress();
	geomDesc.Triangles.VertexBuffer.StrideInBytes = ia.GetVertexBuffer()->GetStructureSize();
	geomDesc.Triangles.VertexCount                = ia.GetVertexBuffer()->GetIndexSize();
	geomDesc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT;
	geomDesc.Triangles.IndexBuffer                = ia.GetIndexBuffer()->GetGPUVirtualAddress();
	geomDesc.Triangles.IndexCount                 = ia.GetIndexBuffer()->GetIndexSize();
	geomDesc.Triangles.IndexFormat                = DXGI_FORMAT_R32_UINT;

	BuildBLAS(geomDesc);

	// bufferRecorderが必要
	Assert(exportGroup->GetExportGroupType() == kExport_Hitgroup); //!< hitgroup以外のexport
	recorder_ = std::make_unique<BufferRecord>(exportGroup);

}

void BottomLevelAS::BindAddress(uint32_t index, const D3D12_GPU_VIRTUAL_ADDRESS address) {
	recorder_->BindAddress(index, address);
}

void BottomLevelAS::BindHandle(uint32_t index, const D3D12_GPU_DESCRIPTOR_HANDLE handle) {
	recorder_->BindHandle(index, handle);
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

void TopLevelAS::Init() {

	//* bufferの生成 *//
	instanceBuffer_
		= std::make_unique<DxObject::BufferResource<D3D12_RAYTRACING_INSTANCE_DESC>>(Sxavenger::GetDevicesObj(), kMaxInstanceNum_);

	//* TLASの構築
	BuildTLAS();

	// RayTracingSRVの生成
	{
		descriptor_ = Sxavenger::GetCurrentDescriptor(DxObject::SRV);

		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.ViewDimension                            = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
		desc.Shader4ComponentMapping                  = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.RaytracingAccelerationStructure.Location = buffers_.asbuffer->GetGPUVirtualAddress();

		auto device = Sxavenger::GetDevice();

		device->CreateShaderResourceView(
			nullptr, &desc, descriptor_.GetCPUHandle()
		);
	}

}

void DxrObject::TopLevelAS::Term() {
	Sxavenger::DeleteDescriptor(descriptor_);
}

void TopLevelAS::BeginSetupInstance() {
	instance_.clear(); // 新しく設定するのでclearしておく
}

void TopLevelAS::EndSetupInstance() {
	// bufferの更新
	SetInstanceBuffer();

	// 更新
	BuildTLAS();

	// RayTracingSRVの生成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.ViewDimension                            = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
		desc.Shader4ComponentMapping                  = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.RaytracingAccelerationStructure.Location = buffers_.asbuffer->GetGPUVirtualAddress();

		auto device = Sxavenger::GetDevice();

		device->CreateShaderResourceView(
			nullptr, &desc, descriptor_.GetCPUHandle()
		);
	}
}

void TopLevelAS::SetInstance(BottomLevelAS* blas, const Matrix4x4& worldMatrix, UINT instanceId) {
	instance_.emplace_back(blas, worldMatrix, instanceId);
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

void TopLevelAS::UpdateTLAS() {

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
	buildASDesc.Inputs.Type          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	buildASDesc.Inputs.DescsLayout   = D3D12_ELEMENTS_LAYOUT_ARRAY;
	buildASDesc.Inputs.NumDescs      = static_cast<UINT>(instance_.size());
	buildASDesc.Inputs.InstanceDescs = instanceBuffer_->GetGPUVirtualAddress();
	buildASDesc.Inputs.Flags
		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;

	// インプレース更新
	buildASDesc.SourceAccelerationStructureData  = buffers_.asbuffer->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData    = buffers_.asbuffer->GetGPUVirtualAddress();
	buildASDesc.ScratchAccelerationStructureData = buffers_.update->GetGPUVirtualAddress();
	
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

void TopLevelAS::SetInstanceBuffer() {

	// 前フレームの分のBuffer情報をclear
	instanceBuffer_->ClearData();

	for (uint32_t index = 0; const auto& instance : instance_) {
		Assert(index < kMaxInstanceNum_); //!< instance数の超過

		// bufferにinstance情報を設定
		auto& desc = (*instanceBuffer_)[index];
		desc.Flags        = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		desc.InstanceMask = 0xFF;

		// matrixを3x4に変更
		Matrix4x4 mat = instance.worldMatrix.Transpose();
		std::memcpy(desc.Transform, &mat, sizeof(desc.Transform));

		desc.AccelerationStructure               = instance.blas->GetGPUVirtualAddress();
		desc.InstanceID                          = instance.instanceId;
		desc.InstanceContributionToHitGroupIndex = index; //!< hack

		index++;
	}
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
	result.scratch = CreateBuffer(
		D3D12_HEAP_TYPE_DEFAULT,
		info.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON
	);

	result.asbuffer = CreateBuffer(
		D3D12_HEAP_TYPE_DEFAULT,
		info.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE
	);

	// アップデート用Bufferの生成
	if (inputs.Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE) {
		result.update = CreateBuffer(
			D3D12_HEAP_TYPE_DEFAULT,
			info.UpdateScratchDataSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_COMMON
		);
	}

	return result;
}

ComPtr<ID3D12Resource> DxrObject::CreateBuffer(
	D3D12_HEAP_TYPE heapType,
	UINT64 bufferSize,
	D3D12_RESOURCE_FLAGS flags,
	D3D12_RESOURCE_STATES initState) {

	Assert(heapType != D3D12_HEAP_TYPE_READBACK && heapType != D3D12_HEAP_TYPE_CUSTOM); //!< 例外
	
	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type             = heapType;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask  = 1;

	// bufferの生成
	ComPtr<ID3D12Resource> result;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width            = bufferSize;
	desc.Height           = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels        = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags            = flags;

	// deviceの取り出し
	auto device = Sxavenger::GetDevice();

	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		initState,
		nullptr,
		IID_PPV_ARGS(&result)
	);

	Assert(SUCCEEDED(hr));

	return result;
}
