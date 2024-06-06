#include "DxrStateObject.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Logger.h>

#include <DxDevices.h>
#include <DxrShaderBlob.h>
#include <DxrRootSignature.h>

#include <d3dx12.h>

// Geometry
#include <Vector4.h>
#include <Vector3.h>
#include <Vector2.h>



////////////////////////////////////////////////////////////////////////////////////////////
// StateObjectDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::StateObjectDesc::Clear() {
	globalRootSignature = nullptr;
	localRootSignatures.clear();
	blob.reset();
}

void DxrObject::StateObjectDesc::CreateLocalRootSignature(uint32_t size) {
	localRootSignatures.resize(size);

	for (uint32_t i = 0; i < size; ++i) {
		localRootSignatures[i] = std::make_unique<LocalRootSignature>();
	}
}

void DxrObject::StateObjectDesc::SetLocalRootSignatureDesc(
	uint32_t index, DxObject::Devices* device, const RootSignatureDesc& desc) {

	localRootSignatures[index]->Init(device, desc);
}

void DxrObject::StateObjectDesc::CreateShadeBlob() {
	blob = std::make_unique<ShaderBlob>();
}

////////////////////////////////////////////////////////////////////////////////////////////
// anonymouse namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace { 

	////////////////////////////////////////////////////////////////////////////////////////////
	// Payload structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Payload {
		int rayType = 0;
		uint32_t reflectionCount;
		Vector4f color;
		int isCollision;
		float length;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Attribute structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Attribute {
		Vector2f attribute;
	};

	/*
	 hlslでのPayload, Attributeの構造体サイズを合わせること
	*/

	////////////////////////////////////////////////////////////////////////////////////////////
	// ShaderHitGroup structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ShaderHitGroup {
		std::wstring closestHit;
		std::wstring intersection;
		std::wstring anyHit;
	};
}


////////////////////////////////////////////////////////////////////////////////////////////
// StateObject class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::StateObject::Init(DxObject::Devices* devices, const StateObjectDesc& descs) {

	CD3DX12_STATE_OBJECT_DESC desc{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };

	// hitgroupの管理
	std::unordered_map<std::wstring, ShaderHitGroup> hitgroups;

	// shaderの設定
	{
		auto dxilLibrarySubobject
			= desc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();

		IDxcBlob* blob = descs.blob->GetShaderBlob();

		D3D12_SHADER_BYTECODE libdxil = {};
		libdxil.BytecodeLength  = blob->GetBufferSize();
		libdxil.pShaderBytecode = blob->GetBufferPointer();

		std::vector<LPCWSTR> exports;

		for (const auto& data : descs.blob->GetDatas()) {
			exports.push_back(data.mainFunctionName.c_str());

			// 取り出しついでにhitgroupの管理
			// todo: anyhit intersection も追加
			if (data.shaderType == ShaderType::CLOSESTHIT_SHADER) {
				assert(hitgroups[data.hitgroup].closestHit.empty()); //!< すでにhitgroupに登録されている
				hitgroups[data.hitgroup].closestHit = data.mainFunctionName;
			}
		}

		dxilLibrarySubobject->DefineExports(exports.data(), static_cast<UINT>(exports.size()));
		dxilLibrarySubobject->SetDXILLibrary(&libdxil);

	}

	// hitGroupの生成
	{
		for (const auto& hitgroup : hitgroups) {
			auto hitGroupSubobject
				= desc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();

			if (!hitgroup.second.closestHit.empty()) {
				hitGroupSubobject->SetClosestHitShaderImport(hitgroup.second.closestHit.c_str());
			}

			if (!hitgroup.second.intersection.empty()) {
				hitGroupSubobject->SetIntersectionShaderImport(hitgroup.second.intersection.c_str());
			}

			if (!hitgroup.second.anyHit.empty()) {
				hitGroupSubobject->SetAnyHitShaderImport(hitgroup.second.anyHit.c_str());
			}

			hitGroupSubobject->SetHitGroupExport(hitgroup.first.c_str());
			hitGroupSubobject->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);
			// todo: hitgrouptypeも選択できるように
		}

		hitgroupCount_ = static_cast<uint32_t>(hitgroups.size()); //!< hitgroupの数の保存
		hitgroups.clear(); //!< 使わなくなったのでクリア
	}

	// shaderConfigの設定
	{
		// TODO: sizeがstateObjectごとに違う場合でも設定可能にしておく

		auto shaderConfig
			= desc.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();

		UINT payloadSize   = sizeof(Payload);
		UINT attributeSize = sizeof(Attribute);

		shaderConfig->Config(payloadSize, attributeSize);
	}


	// GloabalRootSignatureの設定
	{
		auto globalRootSignatureSubobject
			= desc.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();

		globalRootSignatureSubobject->SetRootSignature(descs.globalRootSignature->GetRootSignature());
	}

	// LoaclRootSignatureの設定
	/*{
		// XXX: 自動的にに割り振れるようにしておく

		// raygeneration rootSignature
		auto raygenerationSubobject
			= desc.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();

		raygenerationSubobject->SetRootSignature(descs.localRootSignatures[0]->GetRootSignature());

		// raygeneration symbol の設定
		auto raygenerationSymbolSubobject
			= desc.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();

		LPCWSTR symbols[] = {
			descs.blob->GetMainFunctionName(RAYGENERATION_SHAEAR).c_str()
		};

		raygenerationSymbolSubobject->AddExports(symbols);
		raygenerationSymbolSubobject->SetSubobjectToAssociate(*raygenerationSubobject);


		// closestHit local rootSignature
		auto closestHitSubobject
			= desc.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();

		closestHitSubobject->SetRootSignature(descs.localRootSignatures[1]->GetRootSignature());

		auto hitGroupSymbolSubobject
			= desc.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();

		hitGroupSymbolSubobject->AddExport(kHitGroup);
		hitGroupSymbolSubobject->SetSubobjectToAssociate(*closestHitSubobject);

	}*/

	// localRootSignatureの設定
	{
		for (const auto& local : descs.localRootSignatures) {
			assert(local->GetExportType() != ExportType::kNotSelectExport); //!< localRootSignatureに設定できない

			auto localRootSigantureSubobject
				= desc.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();

			localRootSigantureSubobject->SetRootSignature(local->GetRootSignature());

			auto exportSubobject
				= desc.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();

			exportSubobject->AddExport(local->GetExportName().c_str());
			exportSubobject->SetSubobjectToAssociate(*localRootSigantureSubobject);
		}
	}

	// pipelineConfigの設定
	{
		auto pipelineConfigSubobject
			= desc.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();

		uint32_t maxRecursionDepth = 16; //!< 最大31回

		pipelineConfigSubobject->Config(maxRecursionDepth);
	}

	auto hr = devices->GetDevice()->CreateStateObject(
		desc,
		IID_PPV_ARGS(&stateObject_)
	);

	/*assert(SUCCEEDED(hr));*/
	AssertHRESULT(hr);

	stateObject_.As(&properties_);
}

void DxrObject::StateObject::Term() {
}