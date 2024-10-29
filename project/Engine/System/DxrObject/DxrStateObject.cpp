#include "DxrStateObject.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>

//* c++
#include <sstream>
#include <iomanip>

////////////////////////////////////////////////////////////////////////////////////////////
// StateObject class methods
////////////////////////////////////////////////////////////////////////////////////////////

void StateObject::Term() {
}

void StateObject::CreateRootSignature(const GlobalRootSignatureDesc& desc) {
	globalRootSignature_ = desc.CreateRootSignature(Sxavenger::GetDevice());
}

void StateObject::CreateStateObject(const RaytracingShaderBlobGroup* blobGroup) {
	//!< HACK: PIXで使えない

	CD3DX12_STATE_OBJECT_DESC desc;
	desc.SetStateObjectType(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

	CreateDxilLibrary(desc, blobGroup);
	CreateGlobalRootSignature(desc);
	CreateLocalRootSignature(desc, blobGroup);
	CreateShaderConfig(desc);
	CreatePipelineConfig(desc);

	// deviceの取り出し
	auto device = Sxavenger::GetDevice();

	// stateObjectの生成
	auto hr = device->CreateStateObject(
		desc,
		IID_PPV_ARGS(stateObject_.GetAddressOf())
	);
	Assert(SUCCEEDED(hr));

	stateObject_.As(&properties_);

}

void StateObject::SetStateObject() {

	auto commandList = Sxavenger::GetCommandList();

	commandList->SetComputeRootSignature(globalRootSignature_.Get());
	commandList->SetPipelineState1(stateObject_.Get());
}

void StateObject::CreateDxilLibrary(CD3DX12_STATE_OBJECT_DESC& desc, const RaytracingShaderBlobGroup* blobGroup) {

	for (const auto blob : blobGroup->GetBlobs()) {

		auto dxilLibrarySubobject
			= desc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();

		// bytecodeの設定
		D3D12_SHADER_BYTECODE bytecode = {};
		bytecode.BytecodeLength  = blob->GetBlob()->GetBufferSize();
		bytecode.pShaderBytecode = blob->GetBlob()->GetBufferPointer();

		dxilLibrarySubobject->SetDXILLibrary(&bytecode);

		// export設定
		std::vector<LPCWSTR> entrypoints;

		for (const auto expt : blob->GetExports()) {
			switch (expt->GetExportType()) {
				case ExportGroupType::kExportGroup_Raygeneration: {

						// raygenerationのentrypointを取得
						const auto& entrypoint = expt->GetName();

						// define entry pointに設定
						Assert(!entrypoint.empty(), "not set raygeneration entry point.");
						entrypoints.emplace_back(entrypoint.c_str());

						
					}
					break;

				case ExportGroupType::kExportGroup_Miss: {

						// missのentrypointの取得
						const auto& entrypoint = expt->GetName();

						// define entry pointに設定
						Assert(!entrypoint.empty(), "not set miss entry point.");
						entrypoints.emplace_back(entrypoint.c_str());
					}
					break;

				case ExportGroupType::kExportGroup_Hitgroup: {

						// hitgroupのentrypointの取得
						const auto& hitgroupEntrypoints = expt->GetExportHitgroupEntryPoint();

						// closesthit
						Assert(!hitgroupEntrypoints.closesthit.empty());
						entrypoints.emplace_back(hitgroupEntrypoints.closesthit.c_str());

						// anyhit
						if (!hitgroupEntrypoints.anyhit.empty()) {
							entrypoints.emplace_back(hitgroupEntrypoints.anyhit.c_str());
						}

						// intersection
						if (!hitgroupEntrypoints.intersection.empty()) {
							entrypoints.emplace_back(hitgroupEntrypoints.intersection.c_str());
						}
					}
					break;

				default:
					Assert(false, "ExportGroup not set type.");
					break;
			}
		}

		dxilLibrarySubobject->DefineExports(entrypoints.data(), static_cast<UINT>(entrypoints.size()));
	}
}

void StateObject::CreateGlobalRootSignature(CD3DX12_STATE_OBJECT_DESC& desc) {

	auto globalRootSignatureSubobject
		= desc.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();

	globalRootSignatureSubobject->SetRootSignature(globalRootSignature_.Get());

}

void StateObject::CreateLocalRootSignature(CD3DX12_STATE_OBJECT_DESC& desc, const RaytracingShaderBlobGroup* blobGroup) {

	// parameterの初期化
	maxBufferSizes_ = {};
	exportCounts_   = {};

	for (const auto blob : blobGroup->GetBlobs()) {
		for (const auto expt : blob->GetExports()) {

			AddExportCount(expt->GetExportType());

			// hitgroupの設定
			if (expt->GetExportType() == ExportGroupType::kExportGroup_Hitgroup) {

				auto hitgroupSubobject
					= desc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();

				hitgroupSubobject->SetHitGroupExport(expt->GetName().c_str());
				hitgroupSubobject->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);
				// todo: AABBへの対応

				// hitgroupのentrypointの取得
				const auto& hitgroupEntrypoints = expt->GetExportHitgroupEntryPoint();

				// closesthit
				Assert(!hitgroupEntrypoints.closesthit.empty());
				hitgroupSubobject->SetClosestHitShaderImport(hitgroupEntrypoints.closesthit.c_str());

				// anyhit
				if (!hitgroupEntrypoints.anyhit.empty()) {
					hitgroupSubobject->SetAnyHitShaderImport(hitgroupEntrypoints.anyhit.c_str());
				}

				// intersection
				if (!hitgroupEntrypoints.intersection.empty()) {
					hitgroupSubobject->SetIntersectionShaderImport(hitgroupEntrypoints.intersection.c_str());
				}
			}

			if (expt->GetLocalRootSignature() == nullptr) {
				continue;
			}

			// localRootSignatureの設定
			auto localRootSignatureSubobject
				= desc.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();

			localRootSignatureSubobject->SetRootSignature(expt->GetLocalRootSignature()->GetRootSignature());

			// localRootSignatureの関連付け
			auto exportSubobject
				= desc.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();

			exportSubobject->AddExport(expt->GetName().c_str());
			exportSubobject->SetSubobjectToAssociate(*localRootSignatureSubobject);

			ReplaceMaxBufferSize(expt->GetExportType(), expt->GetLocalRootSignature());
		}
	}

}

void StateObject::CreateShaderConfig(CD3DX12_STATE_OBJECT_DESC& desc) {

	auto pipelineConfigSubobject
		= desc.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();

	pipelineConfigSubobject->Config(kMaxRecursionDepth);
}

void StateObject::CreatePipelineConfig(CD3DX12_STATE_OBJECT_DESC& desc) {

	auto shaderConfigSubobject
		= desc.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();

	UINT payloadSize   = sizeof(RayTracingPayload);
	UINT attributeSize = sizeof(RayTracingAttribute);

	shaderConfigSubobject->Config(payloadSize, attributeSize);
}

void StateObject::AddExportCount(ExportGroupType type) {
	exportCounts_[static_cast<uint32_t>(type)]++;
}

void StateObject::ReplaceMaxBufferSize(ExportGroupType type, LocalRootSignature* rootSignature) {
	if (type == ExportGroupType::kCountOfExportGroupType) {
		return;
	}

	maxBufferSizes_[static_cast<uint32_t>(type)] = std::max(maxBufferSizes_[static_cast<uint32_t>(type)], rootSignature->GetBufferTotalSize());
}

