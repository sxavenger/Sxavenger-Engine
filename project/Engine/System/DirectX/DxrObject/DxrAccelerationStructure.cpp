#include "DxrAccelerationStructure.h"
DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// AccelerationStructureBuffers structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void AccelerationStructureBuffers::Create(
	DxObject::Device* devices,
	const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs) {

	auto device = devices->GetDevice();

	// 必要なメモリ数を求める
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
	device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

	AccelerationStructureBuffers::CreateAccelerationStructureBuffer(devices, info);
	AccelerationStructureBuffers::CreateScratchBuffer(devices, info);
}

void AccelerationStructureBuffers::CreateAccelerationStructureBuffer(DxObject::Device* device, const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO& info) {
	asbuffer = DxObject::Resource::CreateBuffer(
		device,
		D3D12_HEAP_TYPE_DEFAULT,
		info.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE
	);
	asbuffer.SetName(L"Acceleration Structure Buffer | Acceleration Structure Buffer");
}

void AccelerationStructureBuffers::CreateScratchBuffer(DxObject::Device* device, const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO& info) {

	size_t size = info.ScratchDataSizeInBytes;

	if (info.UpdateScratchDataSizeInBytes > 0) {
		size = std::max(size, info.UpdateScratchDataSizeInBytes); //!< update用のscratch bufferも必要な場合, そちらのサイズも考慮する
	}

	scratch = DxObject::Resource::CreateBuffer(
		device,
		D3D12_HEAP_TYPE_DEFAULT,
		size,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON
	);
	scratch.SetName(L"Acceleration Structure Buffer | Scratch Buffer");
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
	buildDesc.Inputs.NumDescs       = 1;
	buildDesc.Inputs.pGeometryDescs = &geomDesc;
	buildDesc.Inputs.Flags
		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	// input情報からbufferの生成
	AccelerationStructureBuffers::Create(device, buildDesc.Inputs);

	// bufferの設定
	buildDesc.ScratchAccelerationStructureData = scratch.GetGPUVirtualAddress();
	buildDesc.DestAccelerationStructureData    = asbuffer.GetGPUVirtualAddress();

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
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	buildASDesc.SourceAccelerationStructureData  = asbuffer.GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData    = asbuffer.GetGPUVirtualAddress();
	buildASDesc.ScratchAccelerationStructureData = scratch.GetGPUVirtualAddress();

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

void TopLevelAS::Init(DxObject::Device* device) {
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
	inputs.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.Flags
		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	AccelerationStructureBuffers::Create(device, inputs);
}

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
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};
	desc.Inputs.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	desc.Inputs.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
	desc.Inputs.NumDescs       = static_cast<UINT>(instances_.size());
	desc.Inputs.InstanceDescs  = descs_->GetGPUVirtualAddress();
	desc.Inputs.Flags
		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	// input情報からbufferの生成
	AccelerationStructureBuffers::Create(device, desc.Inputs);

	// bufferの設定
	desc.ScratchAccelerationStructureData = scratch.GetGPUVirtualAddress();
	desc.DestAccelerationStructureData    = asbuffer.GetGPUVirtualAddress();

	// build descの設定
	context->GetCommandList()->BuildRaytracingAccelerationStructure(
		&desc, 0, nullptr
	);

	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = asbuffer.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);

}

void TopLevelAS::Update(DxObject::CommandContext* context) {

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};
	desc.Inputs.Type          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	desc.Inputs.DescsLayout   = D3D12_ELEMENTS_LAYOUT_ARRAY;
	desc.Inputs.NumDescs      = static_cast<UINT>(instances_.size());
	desc.Inputs.InstanceDescs = descs_->GetGPUVirtualAddress();
	desc.Inputs.Flags
		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	desc.SourceAccelerationStructureData  = asbuffer.GetGPUVirtualAddress();
	desc.DestAccelerationStructureData    = asbuffer.GetGPUVirtualAddress();
	desc.ScratchAccelerationStructureData = scratch.GetGPUVirtualAddress();

	context->GetCommandList()->BuildRaytracingAccelerationStructure(
		&desc, 0, nullptr
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

	if (instances_.size() != previousInstanceCount_) {
		isRequiredBuild = true;
		previousInstanceCount_ = instances_.size();
	}

	descs_->Fill(D3D12_RAYTRACING_INSTANCE_DESC{}); //!< bufferの初期化

	for (size_t i = 0; const auto& instance : instances_) {
		(*descs_)[i].Flags                               = instance.flag.Get();
		(*descs_)[i].InstanceMask                        = instance.instanceMask;
		(*descs_)[i].AccelerationStructure               = instance.bottomLevelAS->GetGPUVirtualAddress();
		(*descs_)[i].InstanceID                          = instance.instanceId;
		(*descs_)[i].InstanceContributionToHitGroupIndex = i; // HACK: 完全に同じobjectの場合, この値は同じになる

		// matrixを3x4に変更
		Matrix4x4 mat = instance.mat.Transpose();
		std::memcpy((*descs_)[i].Transform, &mat, sizeof((*descs_)[i].Transform));

		++i;
	}

	return isRequiredBuild;
}
