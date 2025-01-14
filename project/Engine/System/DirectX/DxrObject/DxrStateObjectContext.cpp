#include "DxrStateObjectContext.h"
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// StateObjectDesc structure
////////////////////////////////////////////////////////////////////////////////////////////

void StateObjectDesc::SetBlob(const DxrObject::RaytracingBlob* blob) {
	blobs.emplace(blob);
}

void StateObjectDesc::SetExportParameter() {
	//for (const auto& blob : blobs) {
	//	for (const auto& expt : blob->GetExports()) {
	//		// todo: ...
	//	}
	//}
}

void StateObjectDesc::SetPayloadStride(size_t stride) {
	payloadStride = stride;
}

void StateObjectDesc::SetAttributeSize(size_t stride) {
	attributeStride = stride;
}

void StateObjectDesc::SetMaxRecursionDepth(uint8_t depth) {
	Assert(depth > 0 && depth < D3D12_RAYTRACING_MAX_DECLARABLE_TRACE_RECURSION_DEPTH, "recursion depth is out of range.");
	maxRecursionDepth = depth;
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
	Assert(SUCCEEDED(hr));
	// note: DebugLayer = true でPIXを起動するとここでエラーが発生する.

	// propertiesの取得
	stateObject_.As(&properties_);

}

void StateObjectContext::BindDXGILibrarySubobject(CD3DX12_STATE_OBJECT_DESC& desc) {
	for (const auto& blob : desc_.blobs) {
		// subobjectの作成
		auto subobject = desc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();

		// blobの設定
		D3D12_SHADER_BYTECODE bytecode = blob->GetBytecode();
		subobject->SetDXILLibrary(&bytecode);

		// entry pointの設定
		std::vector<LPCWSTR> entries = {};

		// exportからentryを取得
		for (const auto& expt : blob->GetExports()) {
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

					entries.emplace_back(hitgroup.closestHit.c_str());

					if (!hitgroup.anyHit.empty()) {
						entries.emplace_back(hitgroup.anyHit.c_str());
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
	for (const auto& blob : desc_.blobs) {
		for (const auto& expt : blob->GetExports()) {

			// hitgroupの場合
			if (expt->GetType() == ExportType::Hitgroup) {

				// hitgroupのexportを設定
				auto hitgroupSubobject = desc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
				hitgroupSubobject->SetHitGroupExport(expt->GetName().c_str());
				hitgroupSubobject->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);
				// todo: AABB Geometryの対応

				// entry pointの設定
				const auto& hitgroup = expt->GetHitgroup();

				Assert(!hitgroup.closestHit.empty(), "closest hit is empty.");
				hitgroupSubobject->SetClosestHitShaderImport(hitgroup.closestHit.c_str());

				if (!hitgroup.anyHit.empty()) {
					hitgroupSubobject->SetAnyHitShaderImport(hitgroup.anyHit.c_str());
				}

				if (!hitgroup.intersection.empty()) {
					hitgroupSubobject->SetIntersectionShaderImport(hitgroup.intersection.c_str());
				}
			}

			ID3D12RootSignature* localRootSignature = expt->GetRootSignature();

			if (localRootSignature == nullptr) {
				continue;
			}

			// localRootSignatureの設定
			auto localRootSignatureSubobject = desc.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
			localRootSignatureSubobject->SetRootSignature(localRootSignature);

			// export関連付け
			auto exportSubobject = desc.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
			exportSubobject->AddExport(expt->GetName().c_str());
			exportSubobject->SetSubobjectToAssociate(*localRootSignatureSubobject);
		}
	}
}

void StateObjectContext::BindConfigsSubobject(CD3DX12_STATE_OBJECT_DESC& desc) {

	auto pipelineConfigSubobject = desc.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	pipelineConfigSubobject->Config(desc_.maxRecursionDepth);

	auto shaderConfigSubobject = desc.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	shaderConfigSubobject->Config(static_cast<UINT>(desc_.payloadStride), static_cast<UINT>(desc_.attributeStride));

}
