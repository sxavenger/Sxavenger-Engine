#include "DxrStateObject.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Sxavenger.h>

//* DirectX12
#include <d3dx12.h>



////////////////////////////////////////////////////////////////////////////////////////////
// StateObject class methods
////////////////////////////////////////////////////////////////////////////////////////////

void StateObject::CreateRootSignature(const GlobalRootSignatureDesc& desc) {
	globalRootSignature_ = desc.CreateRootSignature(Sxavenger::GetDevice());
}

void StateObject::CreateStateObject(const ShaderBlob* blobs) {
	
	CD3DX12_STATE_OBJECT_DESC desc;
	desc.SetStateObjectType(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

	// shaderの設定
	{ //!< 複数shaderの対応
		auto dxilLibrarySubobject
			= desc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();

		IDxcBlob* blob = blobs->GetBlob();

		D3D12_SHADER_BYTECODE bytecode = {};
		bytecode.BytecodeLength  = blob->GetBufferSize();
		bytecode.pShaderBytecode = blob->GetBufferPointer();

		// blobに登録されているentryPointをexport設定
		std::vector<LPCWSTR> entryPoints;

		for (auto expt : blobs->GetExports()) {

			// exportTypeからEntryPointの取得
			switch (expt->GetExportGroupType()) {
				case ExportGroupType::kExport_Raygeneration: {
						// raygenerationのentryPointを取得
						const auto& raygenerationEntryPoint = expt->GetName();

						Assert(!raygenerationEntryPoint.empty());
						entryPoints.emplace_back(raygenerationEntryPoint.c_str());
					}
					break;

				case ExportGroupType::kExport_Miss: {
						// raygenerationのentryPointを取得
						const auto& missEntryPoint = expt->GetName();

						Assert(!missEntryPoint.empty());
						entryPoints.emplace_back(missEntryPoint.c_str());
					}
					break;

				case ExportGroupType::kExport_Hitgroup: {
						// hitgroupのentrypointを取得
						const auto& hitgroupEntryPoints = expt->GetExportHitgroupEntryPoints();

						// closesthit
						Assert(!hitgroupEntryPoints.closesthit.empty());
						entryPoints.emplace_back(hitgroupEntryPoints.closesthit.c_str());

						// anyhit
						if (!hitgroupEntryPoints.anyhit.empty()) {
							entryPoints.emplace_back(hitgroupEntryPoints.anyhit.c_str());
						}

						// intersection
						if (!hitgroupEntryPoints.intersection.empty()) {
							entryPoints.emplace_back(hitgroupEntryPoints.intersection.c_str());
						}
					}
					break;

				default:
					Assert(false); //!< exportType未設定
					break;
			}

			exports_.at(expt->GetExportGroupType()).emplace(expt); //!< 重複しない用に保存(重複はERROR)
		}

		dxilLibrarySubobject->SetDXILLibrary(&bytecode);
		dxilLibrarySubobject->DefineExports(entryPoints.data(), static_cast<UINT>(entryPoints.size()));
	}

	// globalRootSignatureの設定
	{
		auto globalRootSignatureSubobject
			= desc.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();

		globalRootSignatureSubobject->SetRootSignature(globalRootSignature_.Get());
	}

	// localRootSignature, hitgroupの設定
	{
		for (uint32_t i = 0; i < ExportGroupType::kCountOfExportGroupType; ++i) {
			for (auto expt : exports_[i]) {

				// hitgroupの設定
				if (i == ExportGroupType::kExport_Hitgroup) { //!< hitgroupの場合

					auto hitgroupSubobject
						= desc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();

					hitgroupSubobject->SetHitGroupExport(expt->GetName().c_str());

					hitgroupSubobject->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);
					// todo: AABBへの対応

					// entryPointの取り出し
					const auto& hitgroupEntryPoints = expt->GetExportHitgroupEntryPoints();

					// closesthit
					Assert(!hitgroupEntryPoints.closesthit.empty());
					hitgroupSubobject->SetClosestHitShaderImport(hitgroupEntryPoints.closesthit.c_str());

					// anyhit
					if (!hitgroupEntryPoints.anyhit.empty()) {
						hitgroupSubobject->SetAnyHitShaderImport(hitgroupEntryPoints.anyhit.c_str());
					}

					// intersection
					if (!hitgroupEntryPoints.intersection.empty()) {
						hitgroupSubobject->SetIntersectionShaderImport(hitgroupEntryPoints.intersection.c_str());
					}
				}

				if (expt->GetLocalRootSignature() == nullptr) {
					continue; //!< localRootSignatureがない場合はスキップ
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

				// recordSizeの保存
				maxTotalBufferSize_[i] = (std::max)(maxTotalBufferSize_[i], expt->GetLocalRootSignature()->GetTotalBufferSize());
			}
		}
	}

	// pipelineConfigの設定
	{
		auto pipelineConfigSubobject
			= desc.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();

		pipelineConfigSubobject->Config(kMaxRecursionDepth);
	}

	// shaderConfigの設定
	{
		auto shaderConfigSubobject
			= desc.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();

		UINT payloadSize   = sizeof(RayTracingPayload);
		UINT attributeSize = sizeof(RayTracingAttribute);

		shaderConfigSubobject->Config(payloadSize, attributeSize);
	}

	// deviceの取り出し
	auto device = Sxavenger::GetDevice();

	// stateObjectの生成
	auto hr = device->CreateStateObject(
		desc,
		IID_PPV_ARGS(&stateObject_)
	);
	Assert(SUCCEEDED(hr));

	stateObject_.As(&properties_);
}

void StateObject::SetPipeline(ID3D12GraphicsCommandList6* commandList) {
	commandList->SetPipelineState1(stateObject_.Get());
	commandList->SetComputeRootSignature(globalRootSignature_.Get());
}