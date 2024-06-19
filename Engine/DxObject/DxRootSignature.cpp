#include "DxRootSignature.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <DxDevices.h>
#include <Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RootSignatureDesc class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::RootSignatureDesc::Resize(uint32_t paramSize, uint32_t samplerSize) {
	params.resize(paramSize);
	ranges.resize(paramSize);
	samplers.resize(samplerSize);

	/*memset(params.data(), 0, sizeof(D3D12_ROOT_PARAMETER) * params.size());
	memset(samplers.data(), 0, sizeof(D3D12_STATIC_SAMPLER_DESC) * samplers.size());*/
}

void DxObject::RootSignatureDesc::Clear() {
	params.clear();
	params.shrink_to_fit();
	samplers.clear();
	samplers.shrink_to_fit();
	ranges.clear();
	ranges.shrink_to_fit();
}

void DxObject::RootSignatureDesc::SetCBV(uint32_t index, ShaderStage stage, UINT shaderRegister) {
	params[index].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
	params[index].ShaderVisibility          = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params[index].Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::RootSignatureDesc::SetSRV(uint32_t index, ShaderStage stage, UINT shaderRegister) {
	ranges[index].BaseShaderRegister                = shaderRegister;
	ranges[index].NumDescriptors                    = 1;
	ranges[index].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	ranges[index].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	params[index].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[index].ShaderVisibility                    = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params[index].DescriptorTable.pDescriptorRanges   = &ranges[index];
	params[index].DescriptorTable.NumDescriptorRanges = 1;
}

void DxObject::RootSignatureDesc::SetSampler(uint32_t index, TextureMode mode, ShaderStage stage, UINT shaderRegiste) {
	samplers[index].Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplers[index].AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[index].AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[index].AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[index].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers[index].MaxLOD           = D3D12_FLOAT32_MAX;
	samplers[index].ShaderRegister   = shaderRegiste;
	samplers[index].ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(stage);
}

////////////////////////////////////////////////////////////////////////////////////////////
// RootSignature class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::RootSignature::Init(Devices* devices, const RootSignatureDesc& descs) {

	// deviceの取り出し
	auto device = devices->GetDevice();

	// descの設定
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	if (!descs.params.empty()) {
		desc.pParameters   = descs.params.data();
		desc.NumParameters = static_cast<UINT>(descs.params.size());
	}

	if (!descs.samplers.empty()) {
		desc.pStaticSamplers   = descs.samplers.data();
		desc.NumStaticSamplers = static_cast<UINT>(descs.samplers.size());
	}

	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> signatureErrorBlob;

	auto hr = D3D12SerializeRootSignature(
		&desc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob,
		&signatureErrorBlob
	);

	if (FAILED(hr)) {
		Log("//-----------------------------------------------------------------------------------------");
		Log("Failed: Create RootSignatuer");
		Log(reinterpret_cast<char*>(signatureErrorBlob->GetBufferPointer()));
		Log("//-----------------------------------------------------------------------------------------");
		assert(false);
	}

	hr = device->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_)
	);

	assert(SUCCEEDED(hr));

	Log("[DxObject.RootSignature]: rootSignature_ << Complete Create");
}

void DxObject::RootSignature::Term() {
}
