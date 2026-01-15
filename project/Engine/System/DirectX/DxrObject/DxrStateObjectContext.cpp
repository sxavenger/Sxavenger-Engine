#include "DxrStateObjectContext.h"
SXAVENGER_ENGINE_USING
DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Convert.h>
#include <Engine/System/Utility/StreamLogger.h>

//* c++
#include <ranges>

////////////////////////////////////////////////////////////////////////////////////////////
// StateObjectDesc structure
////////////////////////////////////////////////////////////////////////////////////////////

void StateObjectDesc::AddExport(const DxrObject::ExportGroup* expt) {
	ExportType type = expt->GetType();
	exports_[static_cast<size_t>(type)].emplace(ToString(expt->GetName()), expt);
	strides_[static_cast<size_t>(type)] = std::max(strides_[static_cast<size_t>(type)], expt->GetBufferStride());
}

void StateObjectDesc::SetPayloadStride(size_t stride) {
	payloadStride = stride;
}

void StateObjectDesc::SetAttributeStride(size_t stride) {
	attributeStride = stride;
}

void StateObjectDesc::SetMaxRecursionDepth(uint8_t depth) {
	StreamLogger::AssertA(depth > 0 && depth < D3D12_RAYTRACING_MAX_DECLARABLE_TRACE_RECURSION_DEPTH, "recursion depth is out of range.");
	maxRecursionDepth = depth;
}

const DxrObject::ExportGroup* StateObjectDesc::GetExport(ExportType type, const std::string& name) const {
	StreamLogger::AssertA(exports_[static_cast<size_t>(type)].contains(name), "export group is not found. name: " + name);
	return exports_[static_cast<size_t>(type)].at(name);
}

////////////////////////////////////////////////////////////////////////////////////////////
// StateObjectContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

void StateObjectContext::CreateRootSignature(DxObject::Device* device, const GlobalRootSignatureDesc& desc) {
	rootSignature_ = desc.CreateGlobalRootSignature(device->GetDevice());
}

void StateObjectContext::CreateStateObject(DxObject::Device* device, const StateObjectDesc& desc) {

	// descの保存
	desc_ = desc;

	// state objectの設定
	CD3DX12_STATE_OBJECT_DESC stateObjectDesc = {};
	stateObjectDesc.SetStateObjectType(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

	// subobjectの設定
	BindDXGILibrarySubobject(stateObjectDesc);
	BindGlobalRootSignatureSubobject(stateObjectDesc);
	BindExportLocalRootSignatureSubobject(stateObjectDesc);
	BindConfigsSubobject(stateObjectDesc);

	// state objectの生成
	auto hr = device->GetDevice()->CreateStateObject(
		stateObjectDesc, IID_PPV_ARGS(&stateObject_)
	);
	DxObject::Assert(hr, L"raytracing state object create failed.");

	// propertiesの取得
	stateObject_.As(&properties_);

}

void StateObjectContext::CreateStateObject(DxObject::Device* device, StateObjectDesc&& desc) {
	// descの保存
	desc_ = std::move(desc);

	// state objectの設定
	CD3DX12_STATE_OBJECT_DESC stateObjectDesc = {};
	stateObjectDesc.SetStateObjectType(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

	// subobjectの設定
	BindDXGILibrarySubobject(stateObjectDesc);
	BindGlobalRootSignatureSubobject(stateObjectDesc);
	BindExportLocalRootSignatureSubobject(stateObjectDesc);
	BindConfigsSubobject(stateObjectDesc);

	// state objectの生成
	auto hr = device->GetDevice()->CreateStateObject(
		stateObjectDesc, IID_PPV_ARGS(&stateObject_)
	);
	DxObject::Assert(hr, L"raytracing state object create failed.");

	// propertiesの取得
	stateObject_.As(&properties_);
}

void StateObjectContext::UpdateShaderTable(
	DxObject::Device* device,
	const DxrObject::TopLevelAS* toplevelAS,
	const DxrObject::WriteBindBufferDesc* raygeneration, const DxrObject::WriteBindBufferDesc* miss) {

	// shader単体のsizeの設定
	UINT raygenerationRecordSize = kShaderRecordSize;
	raygenerationRecordSize     += static_cast<UINT>(desc_.GetStride(ExportType::Raygeneration));
	raygenerationRecordSize      = Alignment(raygenerationRecordSize, kShaderRecordAlignment);

	// miss
	UINT missRecordSize = kShaderRecordSize;
	missRecordSize     += static_cast<UINT>(desc_.GetStride(ExportType::Miss));
	missRecordSize      = Alignment(missRecordSize, kShaderRecordAlignment);

	// hitgroup
	UINT hitgroupRecordSize = kShaderRecordSize;
	hitgroupRecordSize     += static_cast<UINT>(desc_.GetStride(ExportType::Hitgroup));
	hitgroupRecordSize      = Alignment(hitgroupRecordSize, kShaderRecordAlignment);

	// 使用する各シェーダーの個数より、シェーダーテーブルのサイズを求める.
	UINT raygenerationSize = static_cast<UINT>(desc_.GetExportCount(ExportType::Raygeneration)) * raygenerationRecordSize;
	UINT missSize          = static_cast<UINT>(desc_.GetExportCount(ExportType::Miss)) * missRecordSize;
	UINT hitgroupSize      = static_cast<UINT>(toplevelAS->GetInstanceDescCount()) * hitgroupRecordSize;

	// 各テーブル開始位置にアライメント調整
	UINT raygenerationRegion = Alignment(raygenerationSize, kShaderTableAlignment);
	UINT missRegion          = Alignment(missSize,          kShaderTableAlignment);
	UINT hitgroupRegion      = Alignment(hitgroupSize,      kShaderTableAlignment);

	// 合計したtableのサイズ
	UINT tableSize = raygenerationRegion + missRegion + hitgroupRegion;

	if (tableSize > shaderTableStride_ || shaderTable_ == nullptr) { //!< tableのサイズが大きい場合
		shaderTableStride_ = tableSize;
		shaderTable_.Reset();

		// tableのresourceの生成
		shaderTable_ = DxObject::CreateBufferResource(
			device->GetDevice(),
			D3D12_HEAP_TYPE_UPLOAD,
			shaderTableStride_,
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ
		);
	}

	// tableのmap
	void* mapped = nullptr;
	shaderTable_->Map(0, nullptr, &mapped);
	uint8_t* addressStart = static_cast<uint8_t*>(mapped);

	// raygeneration
	{
		uint8_t* address = addressStart;

		for (const auto& expt : desc_.GetExports(ExportType::Raygeneration) | std::views::values) {
			address = WriteExport(address, raygenerationRecordSize, expt, raygeneration);
		}
	}

	// miss
	{
		uint8_t* address = addressStart + raygenerationRegion;

		for (const auto& expt : desc_.GetExports(ExportType::Miss) | std::views::values) {
			address = WriteExport(address, missRecordSize, expt, miss);
		}
	}

	// hitgroup
	{
		uint8_t* address = addressStart + raygenerationRegion + missRegion;

		if (toplevelAS != nullptr) { //!< HACK
			for (const auto& instance : toplevelAS->GetInstances()) {
				address = WriteExport(address, hitgroupRecordSize, ExportType::Hitgroup, instance.name, &instance.parameter);
			}
		}
	}

	shaderTable_->Unmap(0, nullptr);

	// descの設定
	dispatchDesc_ = {};

	auto address = shaderTable_->GetGPUVirtualAddress();

	// raygenerationの設定
	dispatchDesc_.RayGenerationShaderRecord.StartAddress = address;
	dispatchDesc_.RayGenerationShaderRecord.SizeInBytes  = raygenerationSize;
	address += raygenerationRegion;

	// missの設定
	dispatchDesc_.MissShaderTable.StartAddress  = address;
	dispatchDesc_.MissShaderTable.SizeInBytes   = missSize;
	dispatchDesc_.MissShaderTable.StrideInBytes = missRecordSize;
	address += missRegion;

	// hitgroupの設定
	if (toplevelAS->GetInstanceDescCount() > 0) {
		dispatchDesc_.HitGroupTable.StartAddress  = address;
		dispatchDesc_.HitGroupTable.SizeInBytes   = hitgroupSize;
		dispatchDesc_.HitGroupTable.StrideInBytes = hitgroupRecordSize;
		address += hitgroupRegion;
	}
	
}

void StateObjectContext::SetStateObject(DxObject::CommandContext* context) const {
	context->GetCommandList()->SetComputeRootSignature(rootSignature_.Get());
	context->GetCommandList()->SetPipelineState1(stateObject_.Get());
}

void StateObjectContext::DispatchRays(DxObject::CommandContext* context, const Vector2ui& size) const {

	D3D12_DISPATCH_RAYS_DESC desc = dispatchDesc_;
	desc.Width  = size.x;
	desc.Height = size.y;
	desc.Depth  = 1;

	context->GetCommandList()->DispatchRays(&desc);
}

void StateObjectContext::DispatchRays(DxObject::CommandContext* context, const Vector3ui& size) const {

	D3D12_DISPATCH_RAYS_DESC desc = dispatchDesc_;
	desc.Width  = size.x;
	desc.Height = size.y;
	desc.Depth  = size.z;

	context->GetCommandList()->DispatchRays(&desc);

}

void StateObjectContext::BindDXGILibrarySubobject(CD3DX12_STATE_OBJECT_DESC& desc) {

	//!< blobごとにexportを仕分け
	std::unordered_map<const DxrObject::RaytracingBlob*, std::vector<const DxrObject::ExportGroup*>> map = {};

	for (const auto& expt : desc_.GetExports(ExportType::Raygeneration) | std::views::values) {
		map[expt->GetBlob()].emplace_back(expt);
	}

	for (const auto& expt : desc_.GetExports(ExportType::Miss) | std::views::values) {
		map[expt->GetBlob()].emplace_back(expt);
	}

	for (const auto& expt : desc_.GetExports(ExportType::Hitgroup) | std::views::values) {
		map[expt->GetBlob()].emplace_back(expt);
	}

	//!< blobごとにsubobjectを作成
	for (const auto& [blob, exports] : map) {

		// subobjectの作成
		auto subobject = desc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();

		// blobの設定
		D3D12_SHADER_BYTECODE bytecode = blob->GetBytecode();
		subobject->SetDXILLibrary(&bytecode);

		// entry pointの設定
		std::vector<LPCWSTR> entries = {};

		// exportからentryを取得
		for (const auto& expt : exports) {
			switch (expt->GetType()) {
				case ExportType::Raygeneration:
					// raygenerationのentryを取得
					entries.emplace_back(expt->GetName().c_str());
					break;

				case ExportType::Miss:
					// missのentryを取得
					entries.emplace_back(expt->GetName().c_str());
					break;

				case ExportType::Hitgroup:
					// hitgroupのentryを取得
					const auto& hitgroup = expt->GetHitgroup();
					entries.emplace_back(hitgroup.closesthit.c_str());

					if (!hitgroup.anyhit.empty()) {
						entries.emplace_back(hitgroup.anyhit.c_str());
					}

					if (!hitgroup.intersection.empty()) {
						entries.emplace_back(hitgroup.intersection.c_str());
					}
					break;
			}
		}

		subobject->DefineExports(entries.data(), static_cast<UINT>(entries.size()));
	}
}

void StateObjectContext::BindGlobalRootSignatureSubobject(CD3DX12_STATE_OBJECT_DESC& desc) {
	auto subobject = desc.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	subobject->SetRootSignature(rootSignature_.Get());
}

void StateObjectContext::BindExportLocalRootSignatureSubobject(CD3DX12_STATE_OBJECT_DESC& desc) {

	for (const auto& expt : desc_.GetExports(ExportType::Raygeneration) | std::views::values) {
		if (expt->GetRootSignature() == nullptr) {
			continue;
		}

		auto localRootSignatureSubobject = desc.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
		localRootSignatureSubobject->SetRootSignature(expt->GetRootSignature());

		auto exportSubobject = desc.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
		exportSubobject->AddExport(expt->GetName().c_str());
		exportSubobject->SetSubobjectToAssociate(*localRootSignatureSubobject);
	}

	for (const auto& expt : desc_.GetExports(ExportType::Miss) | std::views::values) {
		if (expt->GetRootSignature() == nullptr) {
			continue;
		}

		auto localRootSignatureSubobject = desc.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
		localRootSignatureSubobject->SetRootSignature(expt->GetRootSignature());

		auto exportSubobject = desc.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
		exportSubobject->AddExport(expt->GetName().c_str());
		exportSubobject->SetSubobjectToAssociate(*localRootSignatureSubobject);
	}

	for (const auto& expt : desc_.GetExports(ExportType::Hitgroup) | std::views::values) {

		const auto& hitgroup = expt->GetHitgroup();

		auto hitgroupSubobject = desc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
		hitgroupSubobject->SetHitGroupExport(expt->GetName().c_str());
		hitgroupSubobject->SetHitGroupType(hitgroup.type);

		hitgroupSubobject->SetClosestHitShaderImport(hitgroup.closesthit.c_str());

		if (!hitgroup.anyhit.empty()) {
			hitgroupSubobject->SetAnyHitShaderImport(hitgroup.anyhit.c_str());
		}

		if (!hitgroup.intersection.empty()) {
			hitgroupSubobject->SetIntersectionShaderImport(hitgroup.intersection.c_str());
		}

		if (expt->GetRootSignature() == nullptr) {
			continue;
		}

		auto localRootSignatureSubobject = desc.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
		localRootSignatureSubobject->SetRootSignature(expt->GetRootSignature());

		auto exportSubobject = desc.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
		exportSubobject->AddExport(expt->GetName().c_str());
		exportSubobject->SetSubobjectToAssociate(*localRootSignatureSubobject);
	}

	// todo: 関数でまとめる
}

void StateObjectContext::BindConfigsSubobject(CD3DX12_STATE_OBJECT_DESC& desc) {

	auto pipelineConfigSubobject = desc.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	pipelineConfigSubobject->Config(desc_.maxRecursionDepth);

	auto shaderConfigSubobject = desc.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	shaderConfigSubobject->Config(static_cast<UINT>(desc_.payloadStride), static_cast<UINT>(desc_.attributeStride));

}

UINT StateObjectContext::WriteIdentifier(uint8_t* dst, const void* id) {
	std::memcpy(dst, id, kShaderRecordSize);
	return kShaderRecordSize;
}

UINT StateObjectContext::WriteGPUVirtualAddress(uint8_t* dst, const D3D12_GPU_VIRTUAL_ADDRESS& address) {
	std::memcpy(dst, &address, kBufferStride);
	return kBufferStride;
}

UINT StateObjectContext::WriteGPUHandle(uint8_t* dst, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {
	std::memcpy(dst, &handle, kBufferStride);
	return kBufferStride;
}

uint8_t* StateObjectContext::WriteExport(uint8_t* dst, UINT size, const ExportGroup* expt, const WriteBindBufferDesc* desc) {

	auto end = dst + size;

	// exportのid書き込み
	auto id = properties_->GetShaderIdentifier(expt->GetName().c_str());
	StreamLogger::AssertW(id != nullptr, L"export identifier not found. export name: " + expt->GetName());

	dst += WriteIdentifier(dst, id);

	// descの書き込み
	if (desc == nullptr) {
		return end; //!< bufferがない場合
	}

	for (const auto& buffer : expt->GetTable().GetWriteBuffers(*desc)) {
		StreamLogger::AssertA(buffer.has_value(), "buffer not has value.", "// maybe system error.");
		const auto& value = buffer.value();
		
		if (std::holds_alternative<D3D12_GPU_VIRTUAL_ADDRESS>(value)) {
			dst += WriteGPUVirtualAddress(dst, std::get<D3D12_GPU_VIRTUAL_ADDRESS>(value));

		} else if (std::holds_alternative<D3D12_GPU_DESCRIPTOR_HANDLE>(value)) {
			dst += WriteGPUHandle(dst, std::get<D3D12_GPU_DESCRIPTOR_HANDLE>(value));
		}
	}

	return end;
}

uint8_t* StateObjectContext::WriteExport(uint8_t* dst, UINT size, ExportType type, const std::string& name, const WriteBindBufferDesc* desc) {
	return WriteExport(dst, size, desc_.GetExport(type, name), desc);
}
