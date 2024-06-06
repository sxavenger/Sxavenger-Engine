#include "DxrAccelerationStructure.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>
#include <Logger.h>
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
	const std::wstring& hitgroup
) {
	
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
}

void DxrObject::BottomLevelAS::Term() {
	verticesStrucuturedBuffer_.reset();
	indicesStrucuturedBuffer_.reset();
}

////////////////////////////////////////////////////////////////////////////////////////////
// InstanceDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::InstanceDesc::Resize(uint32_t size) {
	descs.resize(size);
	blasPtrs_.resize(size);
}

void DxrObject::InstanceDesc::Set(
	uint32_t index,
	BottomLevelAS* blas, const Matrix4x4& worldMatrix, uint32_t hitGroupIndex, uint32_t instanceId) {

	descs[index].Flags        = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	descs[index].InstanceMask = 0xFF;
	
	// matrixを4x4から3x4に修正
	Matrix4x4 mat = Matrix::Transpose(worldMatrix);
	memcpy(descs[index].Transform, &mat, sizeof(descs[index].Transform));

	descs[index].AccelerationStructure               = blas->GetGPUVirtualAddress();
	descs[index].InstanceContributionToHitGroupIndex = hitGroupIndex;
	descs[index].InstanceID                          = instanceId;
	
	blasPtrs_[index] = blas;
}

void DxrObject::InstanceDesc::Set(
	uint32_t index,
	BottomLevelAS* blas, const Matrix4x4& worldMatrix, uint32_t instanceId) {

	descs[index].Flags        = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	descs[index].InstanceMask = 0xFF;
	
	// matrixを4x4から3x4に修正
	Matrix4x4 mat = Matrix::Transpose(worldMatrix);
	memcpy(descs[index].Transform, &mat, sizeof(descs[index].Transform));

	descs[index].AccelerationStructure               = blas->GetGPUVirtualAddress();
	descs[index].InstanceContributionToHitGroupIndex = index;
	descs[index].InstanceID                          = instanceId;
	
	blasPtrs_[index] = blas;
}

void DxrObject::InstanceDesc::Clear() {
	descs.clear();
	descs.shrink_to_fit();
}

////////////////////////////////////////////////////////////////////////////////////////////
// TopLevelAS class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::TopLevelAS::Create(const InstanceDesc& instanceDesc) {
	// resourceの生成
	instanceDescBuffer_ = std::make_unique<DxObject::BufferResource<D3D12_RAYTRACING_INSTANCE_DESC>>(MyEngine::GetDevicesObj(), static_cast<uint32_t>(instanceDesc.descs.size()));
	instanceDescBuffer_->Memcpy(instanceDesc.descs.data());

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
	buildASDesc.Inputs.Type          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	buildASDesc.Inputs.DescsLayout   = D3D12_ELEMENTS_LAYOUT_ARRAY;
	buildASDesc.Inputs.Flags         = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	buildASDesc.Inputs.NumDescs      = instanceDescBuffer_->GetIndexSize();
	buildASDesc.Inputs.InstanceDescs = instanceDescBuffer_->GetGPUVirtualAddress();

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
			nullptr, &desc, descriptor_.handleCPU
		);
	}

	blasPtrs_.assign(instanceDesc.blasPtrs_.begin(), instanceDesc.blasPtrs_.end());

}

void DxrObject::TopLevelAS::Term() {
	MyEngine::EraseDescriptor(descriptor_);

	instanceDescBuffer_.reset();
}

void DxrObject::TopLevelAS::Update(const InstanceDesc& instanceDesc) {

	assert(instanceDesc.descs.size() <= instanceDescBuffer_->GetIndexSize()); //!< 初期化で使ったdescより配列のオーバー
	
	// instanceDescの更新
	instanceDescBuffer_->Memcpy(instanceDesc.descs.data());
	// HACK: 初期化で作った配列サイズを超えた分は反映されない
	// -> 動的な配列に対応したBufferResourceクラスの生成
	
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
	buildASDesc.Inputs.Type          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	buildASDesc.Inputs.DescsLayout   = D3D12_ELEMENTS_LAYOUT_ARRAY;
	buildASDesc.Inputs.NumDescs      = instanceDescBuffer_->GetIndexSize();
	buildASDesc.Inputs.InstanceDescs = instanceDescBuffer_->GetGPUVirtualAddress();
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
				D3D12_RESOURCE_STATE_COMMON // before: STATE_UNORDERED_ACCESS
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