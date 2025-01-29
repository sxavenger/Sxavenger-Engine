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
	scratch->SetName(L"AccelerationStructureBuffers::scratch");

	asbuffer = DxObject::CreateBufferResource(
		device,
		D3D12_HEAP_TYPE_DEFAULT,
		info.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE
	);
	asbuffer->SetName(L"AccelerationStructureBuffers::asbuffer");

	if (inputs.Flags & D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE) {
		update = DxObject::CreateBufferResource(
			device,
			D3D12_HEAP_TYPE_DEFAULT,
			info.UpdateScratchDataSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_COMMON
		);
		update->SetName(L"AccelerationStructureBuffers::update");
	}
}


////////////////////////////////////////////////////////////////////////////////////////////
// BottomLevelAS class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BottomLevelAS::Build(
	DxObject::Device* device, const DxObject::CommandContext* context,
	const D3D12_RAYTRACING_GEOMETRY_DESC& geomDesc) {

	// geomDescの保存
	geomDesc_ = geomDesc;

	// build descの設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
	buildDesc.Inputs.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	buildDesc.Inputs.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
	buildDesc.Inputs.Flags          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	buildDesc.Inputs.NumDescs       = 1;
	buildDesc.Inputs.pGeometryDescs = &geomDesc;

	// input情報からbufferの生成
	AccelerationStructureBuffers::Create(device, buildDesc.Inputs);

	// bufferの設定
	buildDesc.ScratchAccelerationStructureData = scratch->GetGPUVirtualAddress();
	buildDesc.DestAccelerationStructureData    = asbuffer->GetGPUVirtualAddress();

	// build descの設定
	context->GetCommandList()->BuildRaytracingAccelerationStructure(
		&buildDesc, 0, nullptr
	);

	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = asbuffer.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void BottomLevelAS::Update(DxObject::CommandContext* context) {

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
	buildASDesc.Inputs.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	buildASDesc.Inputs.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
	buildASDesc.Inputs.NumDescs       = 1;
	buildASDesc.Inputs.pGeometryDescs = &geomDesc_;
	buildASDesc.Inputs.Flags
		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;

	buildASDesc.SourceAccelerationStructureData  = asbuffer->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData    = asbuffer->GetGPUVirtualAddress();
	buildASDesc.ScratchAccelerationStructureData = update->GetGPUVirtualAddress();

	context->GetCommandList()->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr
	);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = asbuffer.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);

}

////////////////////////////////////////////////////////////////////////////////////////////
// TopLevelAS class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TopLevelAS::BeginSetupInstance() {
	instances_.clear();
}

void TopLevelAS::AddInstance(const Instance& instance) {
	instances_.emplace_back(instance);
}

void TopLevelAS::EndSetupInstance(DxObject::Device* device, DxObject::CommandContext* context) {
	if (UpdateInstanceBuffer(device)) { //!< Buildまたは再buildが必要な場合
		Build(device, context);

	} else {
		Update(context);
	}
}

void TopLevelAS::Build(DxObject::Device* device, DxObject::CommandContext* context) {

	// build descの設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
	buildDesc.Inputs.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	buildDesc.Inputs.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
	buildDesc.Inputs.Flags          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	buildDesc.Inputs.NumDescs       = descs_->GetSize();
	buildDesc.Inputs.InstanceDescs  = descs_->GetGPUVirtualAddress();

	// input情報からbufferの生成
	AccelerationStructureBuffers::Create(device, buildDesc.Inputs);

	// bufferの設定
	buildDesc.ScratchAccelerationStructureData = scratch->GetGPUVirtualAddress();
	buildDesc.DestAccelerationStructureData    = asbuffer->GetGPUVirtualAddress();

	// build descの設定
	context->GetCommandList()->BuildRaytracingAccelerationStructure(
		&buildDesc, 0, nullptr
	);

	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = asbuffer.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);

}

void TopLevelAS::Update(DxObject::CommandContext* context) {

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
	buildASDesc.Inputs.Type          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	buildASDesc.Inputs.DescsLayout   = D3D12_ELEMENTS_LAYOUT_ARRAY;
	buildASDesc.Inputs.NumDescs      = descs_->GetSize();
	buildASDesc.Inputs.InstanceDescs = descs_->GetGPUVirtualAddress();
	buildASDesc.Inputs.Flags
		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;

	buildASDesc.SourceAccelerationStructureData  = asbuffer->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData    = asbuffer->GetGPUVirtualAddress();
	buildASDesc.ScratchAccelerationStructureData = update->GetGPUVirtualAddress();

	context->GetCommandList()->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr
	);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = asbuffer.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

bool TopLevelAS::UpdateInstanceBuffer(DxObject::Device* device) {
	bool isRequiredBuild = false;

	if (descs_ == nullptr) {
		descs_ = std::make_unique<DxObject::DimensionBuffer<D3D12_RAYTRACING_INSTANCE_DESC>>();
		descs_->Create(device, 1);
		isRequiredBuild = true;
	}

	if (instances_.size() > descs_->GetSize()) { //!< instanceの数がbufferの数より多い場合
		//!< capacityの拡張
		descs_->Create(device, static_cast<uint32_t>(instances_.size()));
		isRequiredBuild = true;
	}

	descs_->Fill(D3D12_RAYTRACING_INSTANCE_DESC{}); //!< bufferの初期化

	for (uint32_t i = 0; i < instances_.size(); ++i) {
		(*descs_)[i].Flags                               = instances_[i].flag.Get();
		(*descs_)[i].InstanceMask                        = 0xFF;
		(*descs_)[i].AccelerationStructure               = instances_[i].bottomLevelAS->GetGPUVirtualAddress();
		(*descs_)[i].InstanceID                          = instances_[i].instanceId;
		(*descs_)[i].InstanceContributionToHitGroupIndex = i; // HACK: 完全に同じobjectの場合, この値は同じになる

		// matrixを3x4に変更
		Matrix4x4 mat = instances_[i].mat.Transpose();
		std::memcpy((*descs_)[i].Transform, &mat, sizeof((*descs_)[i].Transform));
	}

	return isRequiredBuild;
}
