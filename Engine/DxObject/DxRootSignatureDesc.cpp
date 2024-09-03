#include "DxRootSignatureDesc.h"

////////////////////////////////////////////////////////////////////////////////////////////
// BaseRootSignatureDesc base structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::BaseRootSignatureDesc::Clear() {
	params.clear();
	samplers.clear();
	ranges_.clear();
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
		Assert(false);
	}

	ComPtr<ID3D12RootSignature> result;

	hr = device->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&result)
	);

	Assert(SUCCEEDED(hr));

	return result;
}

void DxObject::BaseRootSignatureDesc::AutoResizeParam(uint32_t index) {
	Assert(index < kMaxParamReserve_); //!< reserve分を超えてないか確認

	if (index < params.size()) { //!< indexがsize以下ならresizeしない
		return;
	}

	params.resize(index + 1);
	ranges_.resize(index + 1);
}

void DxObject::BaseRootSignatureDesc::AutoResizeSampler(uint32_t index) {
	Assert(index < kMaxSamplerReserve_);

	if (index < samplers.size()) { //!< indexがsize以下ならresizeしない
		return;
	}

	samplers.resize(index + 1);
}

void DxObject::BaseRootSignatureDesc::Reserve() {
	params.reserve(kMaxParamReserve_);
	samplers.reserve(kMaxSamplerReserve_);
	ranges_.reserve(kMaxParamReserve_);
}

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsRootSignatureDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::GraphicsRootSignatureDesc::SetCBV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister) {
	AutoResizeParam(index);
	params.at(index).ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
	params.at(index).ShaderVisibility          = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params.at(index).Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::GraphicsRootSignatureDesc::SetVirtualSRV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister) {
	AutoResizeParam(index);
	params.at(index).ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
	params.at(index).ShaderVisibility          = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params.at(index).Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::GraphicsRootSignatureDesc::SetVirtualUAV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister) {
	AutoResizeParam(index);
	params.at(index).ParameterType             = D3D12_ROOT_PARAMETER_TYPE_UAV;
	params.at(index).ShaderVisibility          = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params.at(index).Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::GraphicsRootSignatureDesc::SetSRV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister) {
	AutoResizeParam(index);
	ranges_.at(index).BaseShaderRegister                = shaderRegister;
	ranges_.at(index).NumDescriptors                    = 1;
	ranges_.at(index).RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	ranges_.at(index).OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	params.at(index).ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params.at(index).ShaderVisibility                    = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params.at(index).DescriptorTable.pDescriptorRanges   = &ranges_.at(index);
	params.at(index).DescriptorTable.NumDescriptorRanges = 1;
}

void DxObject::GraphicsRootSignatureDesc::SetUAV(uint32_t index, ShaderVisibility stage, uint32_t shaderRegister) {
	AutoResizeParam(index);
	ranges_.at(index).BaseShaderRegister                = shaderRegister;
	ranges_.at(index).NumDescriptors                    = 1;
	ranges_.at(index).RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	ranges_.at(index).OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	params.at(index).ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params.at(index).ShaderVisibility                    = static_cast<D3D12_SHADER_VISIBILITY>(stage);
	params.at(index).DescriptorTable.pDescriptorRanges   = &ranges_.at(index);
	params.at(index).DescriptorTable.NumDescriptorRanges = 1;
}

void DxObject::GraphicsRootSignatureDesc::SetSampler(uint32_t sampleIndex, SamplerMode mode, ShaderVisibility stage, UINT shaderRegister) {
	AutoResizeSampler(sampleIndex);
	samplers.at(sampleIndex).Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplers.at(sampleIndex).AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers.at(sampleIndex).AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers.at(sampleIndex).AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers.at(sampleIndex).ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers.at(sampleIndex).MaxLOD           = D3D12_FLOAT32_MAX;
	samplers.at(sampleIndex).ShaderRegister   = shaderRegister;
	samplers.at(sampleIndex).ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(stage);
}

void DxObject::GraphicsRootSignatureDesc::SetSamplerAnisotropic(uint32_t sampleIndex, SamplerMode mode, ShaderVisibility stage, UINT shaderRegister, uint32_t anisotropic) {
	AutoResizeSampler(sampleIndex);
	samplers.at(sampleIndex).Filter           = D3D12_FILTER_ANISOTROPIC;
	samplers.at(sampleIndex).MaxAnisotropy    = anisotropic; //!< 異方性フィルタリングパラメーター
	samplers.at(sampleIndex).AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers.at(sampleIndex).AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers.at(sampleIndex).AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers.at(sampleIndex).ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers.at(sampleIndex).MaxLOD           = D3D12_FLOAT32_MAX;
	samplers.at(sampleIndex).ShaderRegister   = shaderRegister;
	samplers.at(sampleIndex).ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(stage);
}

////////////////////////////////////////////////////////////////////////////////////////////
// CSRootSignatureDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::CSRootSignatureDesc::SetCBV(uint32_t index, uint32_t shaderRegister) {
	AutoResizeParam(index);
	params.at(index).ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
	params.at(index).ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
	params.at(index).Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::CSRootSignatureDesc::SetVirtualSRV(uint32_t index, uint32_t shaderRegister) {
	AutoResizeParam(index);
	params.at(index).ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
	params.at(index).ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
	params.at(index).Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::CSRootSignatureDesc::SetVirtualUAV(uint32_t index, uint32_t shaderRegister) {
	AutoResizeParam(index);
	params.at(index).ParameterType             = D3D12_ROOT_PARAMETER_TYPE_UAV;
	params.at(index).ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
	params.at(index).Descriptor.ShaderRegister = shaderRegister;
}

void DxObject::CSRootSignatureDesc::SetSRV(uint32_t index, uint32_t shaderRegister) {
	AutoResizeParam(index);
	ranges_.at(index).BaseShaderRegister                = shaderRegister;
	ranges_.at(index).NumDescriptors                    = 1;
	ranges_.at(index).RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	ranges_.at(index).OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	params.at(index).ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params.at(index).ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
	params.at(index).DescriptorTable.pDescriptorRanges   = &ranges_.at(index);
	params.at(index).DescriptorTable.NumDescriptorRanges = 1;
}

void DxObject::CSRootSignatureDesc::SetUAV(uint32_t index, uint32_t shaderRegister) {
	AutoResizeParam(index);
	ranges_.at(index).BaseShaderRegister                = shaderRegister;
	ranges_.at(index).NumDescriptors                    = 1;
	ranges_.at(index).RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	ranges_.at(index).OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	params.at(index).ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params.at(index).ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
	params.at(index).DescriptorTable.pDescriptorRanges   = &ranges_.at(index);
	params.at(index).DescriptorTable.NumDescriptorRanges = 1;
}

void DxObject::CSRootSignatureDesc::SetSampler(uint32_t sampleIndex, SamplerMode mode, UINT shaderRegister) {
	AutoResizeSampler(sampleIndex);
	samplers.at(sampleIndex).Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplers.at(sampleIndex).AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers.at(sampleIndex).AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers.at(sampleIndex).AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers.at(sampleIndex).ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers.at(sampleIndex).MaxLOD           = D3D12_FLOAT32_MAX;
	samplers.at(sampleIndex).ShaderRegister   = shaderRegister;
	samplers.at(sampleIndex).ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
}

void DxObject::CSRootSignatureDesc::SetSamplerAnisotropic(uint32_t sampleIndex, SamplerMode mode, UINT shaderRegister, uint32_t anisotropic) {
	AutoResizeSampler(sampleIndex);
	samplers.at(sampleIndex).Filter           = D3D12_FILTER_ANISOTROPIC;
	samplers.at(sampleIndex).MaxAnisotropy    = anisotropic; //!< 異方性フィルタリングパラメーター
	samplers.at(sampleIndex).AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers.at(sampleIndex).AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers.at(sampleIndex).AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers.at(sampleIndex).ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers.at(sampleIndex).MaxLOD           = D3D12_FLOAT32_MAX;
	samplers.at(sampleIndex).ShaderRegister   = shaderRegister;
	samplers.at(sampleIndex).ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
}
