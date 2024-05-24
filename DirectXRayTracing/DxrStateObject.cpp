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
		Vector3f color;
		int isLightRay;
		int isCollision;
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
}


////////////////////////////////////////////////////////////////////////////////////////////
// StateObject class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::StateObject::Init(DxObject::Devices* devices, const StateObjectDesc& descs) {

	CD3DX12_STATE_OBJECT_DESC desc{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };

	const LPCWSTR kHitGroup = L"hitGroup";

	// shaderの設定
	{
		auto dxilLibrarySubobject
			= desc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();

		IDxcBlob* blob = descs.blob->GetShaderBlob();

		D3D12_SHADER_BYTECODE libdxil = {};
		libdxil.BytecodeLength  = blob->GetBufferSize();
		libdxil.pShaderBytecode = blob->GetBufferPointer();

		// TODO: raygeneration, closestHit... のメイン関数が別シェーダーにある場合でも動作するようにする
		LPCWSTR exports[] = {
			descs.blob->GetMainFunctionName(RAYGENERATION_SHAEAR).c_str(),
			descs.blob->GetMainFunctionName(CLOSESTHIT_SHADER).c_str(),
			descs.blob->GetMainFunctionName(MISS_SHADER).c_str(),
		};

		dxilLibrarySubobject->DefineExports(exports);
		dxilLibrarySubobject->SetDXILLibrary(&libdxil);

	}

	// hitGroupの生成
	{
		// HACK: hitgroupの生成がユーザー側である程度できるように改良

		auto hitGroupSubobject
			= desc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();

		hitGroupSubobject->SetClosestHitShaderImport(descs.blob->GetMainFunctionName(CLOSESTHIT_SHADER).c_str());
		hitGroupSubobject->SetHitGroupExport(kHitGroup);
		hitGroupSubobject->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);
		/*
			処置として送られてきたhitGroupに関係あるshaderを一つのgroupにまとめておく
		*/
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
	{
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
}

void DxrObject::StateObject::Term() {
}