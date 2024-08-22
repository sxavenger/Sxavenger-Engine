#include "DxRootSignatureDesc.h"

////////////////////////////////////////////////////////////////////////////////////////////
// BaseRootSignatureDesc base structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::BaseRootSignatureDesc::Resize(uint32_t paramSize, uint32_t samplerSize) {
	params.resize(paramSize);
	samplers.resize(samplerSize);
	ranges_.resize(paramSize);
}

void DxObject::BaseRootSignatureDesc::Clear() {
	params.clear();
	params.shrink_to_fit();
	samplers.clear();
	samplers.shrink_to_fit();
	ranges_.clear();
	ranges_.shrink_to_fit();
}

ComPtr<ID3D12RootSignature> DxObject::BaseRootSignatureDesc::CreateRootSignature(ID3D12Device* device) const {

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	if (!params.empty()) {
		desc.pParameters   = params.data();
		desc.NumParameters = static_cast<UINT>(params.size());
	}

	if (!samplers.empty()) {
		desc.pStaticSamplers   = samplers.data();
		desc.NumStaticSamplers = static_cast<UINT>(samplers.size());
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

	ComPtr<ID3D12RootSignature> result;

	hr = device->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&result)
	);

	assert(SUCCEEDED(hr));

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsRootSignatureDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::GraphicsRootSignatureDesc::SetCBV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister) {
	params[index].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
	params[index].ShaderVisibility          = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params[index].Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::GraphicsRootSignatureDesc::SetVirtualSRV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister) {
	params[index].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
	params[index].ShaderVisibility          = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params[index].Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::GraphicsRootSignatureDesc::SetVirtualUAV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister) {
	params[index].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_UAV;
	params[index].ShaderVisibility          = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params[index].Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::GraphicsRootSignatureDesc::SetSRV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister) {
	ranges_[index].BaseShaderRegister                = shaderRegister;
	ranges_[index].NumDescriptors                    = 1;
	ranges_[index].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	ranges_[index].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	params[index].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[index].ShaderVisibility                    = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params[index].DescriptorTable.pDescriptorRanges   = &ranges_[index];
	params[index].DescriptorTable.NumDescriptorRanges = 1;
}

void DxObject::GraphicsRootSignatureDesc::SetUAV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister) {
	ranges_[index].BaseShaderRegister                = shaderRegister;
	ranges_[index].NumDescriptors                    = 1;
	ranges_[index].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	ranges_[index].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	params[index].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[index].ShaderVisibility                    = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params[index].DescriptorTable.pDescriptorRanges   = &ranges_[index];
	params[index].DescriptorTable.NumDescriptorRanges = 1;
}

void DxObject::GraphicsRootSignatureDesc::SetSampler(uint32_t sampleIndex, SamplerMode mode, ShaderVisibility stage, UINT shaderRegister) {
	samplers[sampleIndex].Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplers[sampleIndex].AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers[sampleIndex].MaxLOD           = D3D12_FLOAT32_MAX;
	samplers[sampleIndex].ShaderRegister   = shaderRegister;
	samplers[sampleIndex].ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(stage);
}

void DxObject::GraphicsRootSignatureDesc::SetSamplerAnisotropic(uint32_t sampleIndex, SamplerMode mode, ShaderVisibility stage, UINT shaderRegister, uint32_t anisotropic) {
	samplers[sampleIndex].Filter           = D3D12_FILTER_ANISOTROPIC;
	samplers[sampleIndex].MaxAnisotropy    = anisotropic; //!< 異方性フィルタリングパラメーター
	samplers[sampleIndex].AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers[sampleIndex].MaxLOD           = D3D12_FLOAT32_MAX;
	samplers[sampleIndex].ShaderRegister   = shaderRegister;
	samplers[sampleIndex].ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(stage);
}

////////////////////////////////////////////////////////////////////////////////////////////
// CSRootSignatureDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::CSRootSignatureDesc::SetCBV(uint32_t index, uint32_t shaderRegister) {
	params[index].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
	params[index].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
	params[index].Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::CSRootSignatureDesc::SetVirtualSRV(uint32_t index, uint32_t shaderRegister) {
	params[index].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
	params[index].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
	params[index].Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::CSRootSignatureDesc::SetVirtualUAV(uint32_t index, uint32_t shaderRegister) {
	params[index].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_UAV;
	params[index].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
	params[index].Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::CSRootSignatureDesc::SetSRV(uint32_t index, uint32_t shaderRegister) {
	ranges_[index].BaseShaderRegister                = shaderRegister;
	ranges_[index].NumDescriptors                    = 1;
	ranges_[index].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	ranges_[index].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	params[index].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[index].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
	params[index].DescriptorTable.pDescriptorRanges   = &ranges_[index];
	params[index].DescriptorTable.NumDescriptorRanges = 1;
}

void DxObject::CSRootSignatureDesc::SetUAV(uint32_t index, uint32_t shaderRegister) {
	ranges_[index].BaseShaderRegister                = shaderRegister;
	ranges_[index].NumDescriptors                    = 1;
	ranges_[index].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	ranges_[index].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	params[index].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[index].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
	params[index].DescriptorTable.pDescriptorRanges   = &ranges_[index];
	params[index].DescriptorTable.NumDescriptorRanges = 1;
}

void DxObject::CSRootSignatureDesc::SetSampler(uint32_t sampleIndex, SamplerMode mode, UINT shaderRegister) {
	samplers[sampleIndex].Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplers[sampleIndex].AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers[sampleIndex].MaxLOD           = D3D12_FLOAT32_MAX;
	samplers[sampleIndex].ShaderRegister   = shaderRegister;
	samplers[sampleIndex].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
}

void DxObject::CSRootSignatureDesc::SetSamplerAnisotropic(uint32_t sampleIndex, SamplerMode mode, UINT shaderRegister, uint32_t anisotropic) {
	samplers[sampleIndex].Filter           = D3D12_FILTER_ANISOTROPIC;
	samplers[sampleIndex].MaxAnisotropy    = anisotropic; //!< 異方性フィルタリングパラメーター
	samplers[sampleIndex].AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[sampleIndex].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers[sampleIndex].MaxLOD           = D3D12_FLOAT32_MAX;
	samplers[sampleIndex].ShaderRegister   = shaderRegister;
	samplers[sampleIndex].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
}
