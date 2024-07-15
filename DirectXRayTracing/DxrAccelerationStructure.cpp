#include "DxrAccelerationStructure.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>
#include <Logger.h>
#include <Console.h>
#include <d3dx12.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;
using namespace DxrMethod;

////////////////////////////////////////////////////////////////////////////////////////////
// BottomLevelAS class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::BottomLevelAS::Create(
	DxObject::BufferResource<VertexData>* vertices, DxObject::IndexBufferResource* indices,
	const std::wstring& hitgroup ) {
	
	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = {};
	geomDesc.Type                                 = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geomDesc.Flags                                = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	geomDesc.Triangles.VertexBuffer.StartAddress  = vertices->GetGPUVirtualAddress();
	geomDesc.Triangles.VertexBuffer.StrideInBytes = vertices->GetStructureSize();
	geomDesc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT; // ここのせいで Vector4 が使えなくなるかも
	geomDesc.Triangles.VertexCount                = vertices->GetIndexSize();
	geomDesc.Triangles.IndexBuffer                = indices->GetGPUVirtualAddress();
	geomDesc.Triangles.IndexFormat                = DXGI_FORMAT_R32_UINT;
	geomDesc.Triangles.IndexCount                 = indices->GetIndexSize();

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
	buildASDesc.Inputs.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	buildASDesc.Inputs.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
	buildASDesc.Inputs.Flags          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
	buildASDesc.Inputs.NumDescs       = 1;
	buildASDesc.Inputs.pGeometryDescs = &geomDesc;

	verticesStrucuturedBuffer_ = std::make_unique<DxObject::StructuredBuffer>(vertices);
	indicesStrucuturedBuffer_ = std::make_unique<DxObject::StructuredBuffer>(indices);

	buffers_ = CreateAccelerationStructure(
		buildASDesc
	);

	// bufferの設定
	buildASDesc.ScratchAccelerationStructureData = buffers_.scratch->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData    = buffers_.asbuffer->GetGPUVirtualAddress();

	// コマンドリストに積む
	auto commandList = MyEngine::GetCommandList();

	commandList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr
	);

	// バリアの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags         = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.UAV.pResource = buffers_.asbuffer.Get();

	commandList->ResourceBarrier(1, &barrier);

	// hitgroupの登録
	hitgroup_ = hitgroup;

	// recordBufferの初期化
	recordBuffer_ = std::make_unique<RecordBuffer>();
	recordBuffer_->SetExport(HITGROUP, hitgroup);

	// bufferを設定しておく
	SetHandle(0, indicesStrucuturedBuffer_->GetGPUHandle());
	SetHandle(1, verticesStrucuturedBuffer_->GetGPUHandle());
}

void DxrObject::BottomLevelAS::Create(
	DxObject::BufferResource<VertexData>* vertices, DxObject::IndexBufferResource* indices,
	DxObject::StructuredBuffer* verticesStructuredBuffer, DxObject::StructuredBuffer* indicesStructuredBuffer,
	const std::wstring& hitgroup) {

	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc = {};
	geomDesc.Type                                 = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geomDesc.Flags                                = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	geomDesc.Triangles.VertexBuffer.StartAddress  = vertices->GetGPUVirtualAddress();
	geomDesc.Triangles.VertexBuffer.StrideInBytes = vertices->GetStructureSize();
	geomDesc.Triangles.VertexFormat               = DXGI_FORMAT_R32G32B32_FLOAT; // ここのせいで Vector4 が使えなくなるかも
	geomDesc.Triangles.VertexCount                = vertices->GetIndexSize();
	geomDesc.Triangles.IndexBuffer                = indices->GetGPUVirtualAddress();
	geomDesc.Triangles.IndexFormat                = DXGI_FORMAT_R32_UINT;
	geomDesc.Triangles.IndexCount                 = indices->GetIndexSize();

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
	buildASDesc.Inputs.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	buildASDesc.Inputs.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
	buildASDesc.Inputs.Flags          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
	buildASDesc.Inputs.NumDescs       = 1;
	buildASDesc.Inputs.pGeometryDescs = &geomDesc;

	buffers_ = CreateAccelerationStructure(
		buildASDesc
	);

	// bufferの設定
	buildASDesc.ScratchAccelerationStructureData = buffers_.scratch->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData    = buffers_.asbuffer->GetGPUVirtualAddress();

	// コマンドリストに積む
	auto commandList = MyEngine::GetCommandList();

	commandList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr
	);

	// バリアの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags         = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.UAV.pResource = buffers_.asbuffer.Get();

	commandList->ResourceBarrier(1, &barrier);

	// hitgroupの登録
	hitgroup_ = hitgroup;

	// recordBufferの初期化
	recordBuffer_ = std::make_unique<RecordBuffer>();
	recordBuffer_->SetExport(HITGROUP, hitgroup);

	// bufferを設定しておく
	SetHandle(0, indicesStructuredBuffer->GetGPUHandle());
	SetHandle(1, verticesStructuredBuffer->GetGPUHandle());

}

void DxrObject::BottomLevelAS::Term() {
	verticesStrucuturedBuffer_.reset();
	indicesStrucuturedBuffer_.reset();
	recordBuffer_.reset();
}

////////////////////////////////////////////////////////////////////////////////////////////
// InstanceBuffer structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::InstanceBuffer::Init() {
	// buffer
	descBuffer_ = std::make_unique<DxObject::DynamicBufferResource<D3D12_RAYTRACING_INSTANCE_DESC>>(MyEngine::GetDevicesObj());
}

void DxrObject::InstanceBuffer::Term() {
}

void DxrObject::InstanceBuffer::SetInstance(
	uint32_t index,
	BottomLevelAS* blas, const Matrix4x4& worldMatrix, uint32_t instanceId) {

	descBuffer_->operator[](index).Flags        = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	descBuffer_->operator[](index).InstanceMask = 0xFF;
	
	// matrixを4x4から3x4に修正
	Matrix4x4 mat = worldMatrix.Transpose();
	std::memcpy(descBuffer_->operator[](index).Transform, &mat, sizeof(descBuffer_->operator[](index).Transform));

	descBuffer_->operator[](index).AccelerationStructure               = blas->GetGPUVirtualAddress();
	descBuffer_->operator[](index).InstanceContributionToHitGroupIndex = index; // hack
	descBuffer_->operator[](index).InstanceID                          = instanceId;

}

void DxrObject::InstanceBuffer::Clear() {
	descBuffer_->Clear();
}

////////////////////////////////////////////////////////////////////////////////////////////
// TopLevelAS class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::TopLevelAS::Init() {

	// instanceBufferの初期化
	instanceBuffer_ = std::make_unique<InstanceBuffer>();

	// asBufferの生成
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
	buildASDesc.Inputs.Type          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	buildASDesc.Inputs.DescsLayout   = D3D12_ELEMENTS_LAYOUT_ARRAY;
	buildASDesc.Inputs.Flags         = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	buildASDesc.Inputs.NumDescs      = instanceBuffer_->GetMaxIndexSize();
	buildASDesc.Inputs.InstanceDescs = instanceBuffer_->GetGPUVirtualAddress();

	buffers_ = CreateAccelerationStructure(
		buildASDesc
	);

	buildASDesc.ScratchAccelerationStructureData = buffers_.scratch->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData    = buffers_.asbuffer->GetGPUVirtualAddress();

	// コマンドに積む
	auto commandList = MyEngine::GetCommandList();

	commandList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr
	);

	// バリアの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags         = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.UAV.pResource = buffers_.asbuffer.Get();

	commandList->ResourceBarrier(1, &barrier);

	// StructredBufferの設定
	{
		descriptor_ = MyEngine::GetCurrentDescripor(SRV);
		
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.ViewDimension                            = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
		desc.Shader4ComponentMapping                  = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.RaytracingAccelerationStructure.Location = buffers_.asbuffer->GetGPUVirtualAddress();

		auto device = MyEngine::GetDevice();

		device->CreateShaderResourceView(
			nullptr, &desc, descriptor_.GetCPUHandle()
		);
	}
}

void DxrObject::TopLevelAS::Term() {
	MyEngine::EraseDescriptor(descriptor_);

	instanceBuffer_.reset();
}

void DxrObject::TopLevelAS::StartBlasSetup() {
	instances_.clear(); //!< 再度TLASへの登録が始まるため要素の削除
}

void DxrObject::TopLevelAS::EndBlasSetup() {
	
	// instanceDescの更新
	SetInstanceBuffer();
	
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
	buildASDesc.Inputs.Type          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	buildASDesc.Inputs.DescsLayout   = D3D12_ELEMENTS_LAYOUT_ARRAY;
	buildASDesc.Inputs.NumDescs      = instanceBuffer_->GetMaxIndexSize();
	buildASDesc.Inputs.InstanceDescs = instanceBuffer_->GetGPUVirtualAddress();
	buildASDesc.Inputs.Flags
		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;

	// インプレース更新
	buildASDesc.SourceAccelerationStructureData  = buffers_.asbuffer->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData    = buffers_.asbuffer->GetGPUVirtualAddress();
	buildASDesc.ScratchAccelerationStructureData = buffers_.update->GetGPUVirtualAddress();
	
	// コマンドに積む
	auto commandList = MyEngine::GetCommandList();

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

void DxrObject::TopLevelAS::SetBLAS(
	BottomLevelAS* blas, const Matrix4x4& worldMatrix, uint32_t instanceId) {

	instances_.push_back({ blas, worldMatrix, instanceId });
}

void DxrObject::TopLevelAS::SetInstanceBuffer() {

	// 前の分のbufferの削除
	instanceBuffer_->Clear();
	topRecordSize_ = 0;

	uint32_t index = 0;
	
	// instances配列をbufferに設定
	for (const auto& instance : instances_) {
		instanceBuffer_->SetInstance(
			index, instance.blas, instance.worldMatrix, instance.instanceId
		);

		// recordサイズの更新
		topRecordSize_ = (std::max)(topRecordSize_, instance.blas->GetRecordSize());

		index++;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// DxrMethod namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

DxrObject::AccelerationStructuredBuffers DxrMethod::CreateAccelerationStructure(
	const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& desc) {

	auto device = MyEngine::GetDevice();

	DxrObject::AccelerationStructuredBuffers result = {};

	// 必要なメモリ数を求める
	{
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
		device->GetRaytracingAccelerationStructurePrebuildInfo(
			&desc.Inputs, &info
		);

		result.scratch = CreateBuffer(
			D3D12_HEAP_TYPE_DEFAULT,
			info.ScratchDataSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_COMMON // D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);

		result.asbuffer = CreateBuffer(
			D3D12_HEAP_TYPE_DEFAULT,
			info.ResultDataMaxSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE
		);

		if (result.scratch == nullptr || result.asbuffer == nullptr) {
			Log(std::format("scratch: {:p}, asbuffer: {:p}", static_cast<void*>(result.scratch.Get()), static_cast<void*>(result.asbuffer.Get())));
			Log("DxrMethod::CreateAccelerationStructure faild.");
			assert(false);
		}

		// アップデート用バッファを確保
		if (desc.Inputs.Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE) {
			result.update = CreateBuffer(
				D3D12_HEAP_TYPE_DEFAULT,
				info.UpdateScratchDataSizeInBytes,
				D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
				D3D12_RESOURCE_STATE_COMMON // before: D3D12_RESOURCE_STATE_UNORDERED_ACCESS
			);

			if (result.update == nullptr) {
				Log(std::format("update: {:p}", static_cast<void*>(result.update.Get())));
				Log("DxrMethod::CreateAccelerationStructure faild.");
				assert(false);
			}
		}
	}

	return result;
}