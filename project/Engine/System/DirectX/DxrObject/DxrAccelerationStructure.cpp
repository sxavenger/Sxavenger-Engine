#include "DxrAccelerationStructure.h"
DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// AccelerationStructureBuffers structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void AccelerationStructureBuffers::Create(
	DxObject::Device* device,
	const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs) {

	// 必要なメモリ数を求める
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
	device->GetDevice()->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

	AccelerationStructureBuffers::CreateAccelerationStructureBuffer(device, info);
	AccelerationStructureBuffers::CreateScratchBuffer(device, info);
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
	scratch = DxObject::Resource::CreateBuffer(
		device,
		D3D12_HEAP_TYPE_DEFAULT,
		std::max(info.ScratchDataSizeInBytes, info.UpdateScratchDataSizeInBytes), //!< update用のscratchのサイズも考慮する
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
	const D3D12_RAYTRACING_GEOMETRY_DESC& geometry) {

	// geometryの保存
	geometry_ = geometry;

	// input情報の設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
	inputs.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs       = 1;
	inputs.pGeometryDescs = &geometry_;
	inputs.Flags
		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	// input情報からbufferの生成
	AccelerationStructureBuffers::Create(device, inputs);

	// build descの設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC build = {};
	build.Inputs                           = inputs;
	build.ScratchAccelerationStructureData = scratch.GetGPUVirtualAddress();
	build.DestAccelerationStructureData    = asbuffer.GetGPUVirtualAddress();

	// buildの実行
	context->GetCommandList()->BuildRaytracingAccelerationStructure(
		&build, 0, nullptr
	);

	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = asbuffer.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void BottomLevelAS::Update(DxObject::CommandContext* context) {

	// input情報の設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
	inputs.Type           = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout    = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs       = 1;
	inputs.pGeometryDescs = &geometry_;
	inputs.Flags
		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	// build descの設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC build = {};
	build.Inputs                           = inputs;
	build.SourceAccelerationStructureData  = asbuffer.GetGPUVirtualAddress();
	build.DestAccelerationStructureData    = asbuffer.GetGPUVirtualAddress();
	build.ScratchAccelerationStructureData = scratch.GetGPUVirtualAddress();

	// buildの実行
	context->GetCommandList()->BuildRaytracingAccelerationStructure(
		&build, 0, nullptr
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

	// input情報の設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
	inputs.Type          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout   = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs      = static_cast<UINT>(instances_.size());
	inputs.InstanceDescs = descs_->GetGPUVirtualAddress();
	inputs.Flags
		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	// input情報からbufferの生成
	AccelerationStructureBuffers::Create(device, inputs);

	// build descの設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC build = {};
	build.Inputs                           = inputs;
	build.ScratchAccelerationStructureData = scratch.GetGPUVirtualAddress();
	build.DestAccelerationStructureData    = asbuffer.GetGPUVirtualAddress();

	// buildの実行
	context->GetCommandList()->BuildRaytracingAccelerationStructure(
		&build, 0, nullptr
	);

	// barrierの設定
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = asbuffer.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);

}

void TopLevelAS::Update(DxObject::CommandContext* context) {

	// input情報の設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
	inputs.Type          = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout   = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs      = static_cast<UINT>(instances_.size());
	inputs.InstanceDescs = descs_->GetGPUVirtualAddress();
	inputs.Flags
		= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE
		| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	// build descの設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC build = {};
	build.Inputs                           = inputs;
	build.SourceAccelerationStructureData  = asbuffer.GetGPUVirtualAddress();
	build.DestAccelerationStructureData    = asbuffer.GetGPUVirtualAddress();
	build.ScratchAccelerationStructureData = scratch.GetGPUVirtualAddress();

	// buildの実行
	context->GetCommandList()->BuildRaytracingAccelerationStructure(
		&build, 0, nullptr
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
