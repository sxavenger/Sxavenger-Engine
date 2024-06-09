#include "DxrRootSignature.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <DxDevices.h>
#include <Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RootSignatureDesc class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::RootSignatureDesc::Resize(uint32_t paramSize, uint32_t samplerSize) {
	params.resize(paramSize);
	ranges.resize(paramSize);
	samplers.resize(samplerSize);
}

void DxrObject::RootSignatureDesc::Clear() {
	params.clear();
	params.shrink_to_fit();
	samplers.clear();
	samplers.shrink_to_fit();
	ranges.clear();
	ranges.shrink_to_fit();
}

void DxrObject::RootSignatureDesc::SetCBV(uint32_t index, UINT shaderRegister) {

	// paramの設定
	params[index].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
	params[index].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
	params[index].Descriptor.ShaderRegister = shaderRegister;
}

void DxrObject::RootSignatureDesc::SetSRV(uint32_t index, UINT shaderRegister) {

	// rangeの設定
	ranges[index].BaseShaderRegister                = shaderRegister;
	ranges[index].NumDescriptors                    = 1;
	ranges[index].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	ranges[index].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// paramの設定
	params[index].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[index].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
	params[index].DescriptorTable.pDescriptorRanges   = &ranges[index];
	params[index].DescriptorTable.NumDescriptorRanges = 1;
}

void DxrObject::RootSignatureDesc::SetUAV(uint32_t index, UINT shaderRegister) {

	// rangeの設定
	ranges[index].BaseShaderRegister                = shaderRegister;
	ranges[index].NumDescriptors                    = 1;
	ranges[index].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	ranges[index].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// paramの設定
	params[index].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[index].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
	params[index].DescriptorTable.pDescriptorRanges   = &ranges[index];
	params[index].DescriptorTable.NumDescriptorRanges = 1;
}

void DxrObject::RootSignatureDesc::SetSampler(uint32_t index, TextureMode mode, UINT shaderRegiste) {

	samplers[index].Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplers[index].AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[index].AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[index].AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers[index].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers[index].MaxLOD           = D3D12_FLOAT32_MAX;
	samplers[index].ShaderRegister   = shaderRegiste;
	samplers[index].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
}

void DxrObject::RootSignatureDesc::SetLocalRootSignature(ExportType type, const std::wstring& exportStr) {

	assert(type != ExportType::kNotSelectExport); //!< localRootSignatureに設定できない
	assert(!exportStr.empty());                   //!< 未入力

	exportType = type;
	exportName = exportStr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// RootSignature class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::RootSignature::Init(DxObject::Devices* devices, const RootSignatureDesc& descs) {
	
	// descの設定
	D3D12_ROOT_SIGNATURE_DESC desc = {};

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
		Log("Failed: Create RayTracing RootSignatuer");
		Log(reinterpret_cast<char*>(signatureErrorBlob->GetBufferPointer()));
		Log("//-----------------------------------------------------------------------------------------");
		assert(false);
	}

	auto device = devices->GetDevice();

	hr = device->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_)
	);

	assert(SUCCEEDED(hr));

	Log("[DxrObject::RootSignature]: rootSignature_ << Complete Create");

}

void DxrObject::RootSignature::Term() {
}

////////////////////////////////////////////////////////////////////////////////////////////
// LocalRootSignature class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::LocalRootSignature::Init(DxObject::Devices* devices, const RootSignatureDesc& descs) {

	// descの設定
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

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
		Log("Failed: Create RayTracing LocalRootSignatuer");
		Log(reinterpret_cast<char*>(signatureErrorBlob->GetBufferPointer()));
		Log("//-----------------------------------------------------------------------------------------");
		assert(false);
	}

	auto device = devices->GetDevice();

	hr = device->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_)
	);

	assert(SUCCEEDED(hr));

	Log("[DxrObject::LoaclRootSignature]: rootSignature_ << Complete Create");

	// 引数不足
	assert(!descs.exportName.empty() || descs.exportType != kNotSelectExport);

	exportType_ = descs.exportType;
	exportName_ = descs.exportName;

}
