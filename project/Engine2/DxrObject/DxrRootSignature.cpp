#include "DxrRootSignature.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// LocalRootSignatureDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void LocalRootSignatureDesc::Clear() {
	GlobalRootSignatureDesc::Clear();

	table_.clear();
}

ComPtr<ID3D12RootSignature> LocalRootSignatureDesc::CreateRootSignature(ID3D12Device* device) const {

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

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

void LocalRootSignatureDesc::SetCBV(uint32_t index, uint32_t shaderRegister) {
	GlobalRootSignatureDesc::SetCBV(index, shaderRegister);

	AutoTableResize(index);
	table_.at(index) = GPUAddress{};
}

void LocalRootSignatureDesc::SetVirtualSRV(uint32_t index, uint32_t shaderRegister) {
	GlobalRootSignatureDesc::SetVirtualSRV(index, shaderRegister);

	AutoTableResize(index);
	table_.at(index) = GPUAddress{};
}

void LocalRootSignatureDesc::SetVirtualUAV(uint32_t index, uint32_t shaderRegister) {
	GlobalRootSignatureDesc::SetVirtualUAV(index, shaderRegister);

	AutoTableResize(index);
	table_.at(index) = GPUAddress{};
}

void LocalRootSignatureDesc::SetSRV(uint32_t index, uint32_t shaderRegister) {
	GlobalRootSignatureDesc::SetSRV(index, shaderRegister);

	AutoTableResize(index);
	table_.at(index) = GPUHandle{};
}

void LocalRootSignatureDesc::SetUAV(uint32_t index, uint32_t shaderRegister) {
	GlobalRootSignatureDesc::SetUAV(index, shaderRegister);

	AutoTableResize(index);
	table_.at(index) = GPUHandle{};
}

void LocalRootSignatureDesc::Reserve() {
	GlobalRootSignatureDesc::Reserve();
	table_.reserve(kMaxParamReserve_);
}

void LocalRootSignatureDesc::AutoTableResize(uint32_t index) {
	Assert(index < kMaxParamReserve_);

	if (index < table_.size()) {
		return;
	}

	table_.resize(index + 1);
}

////////////////////////////////////////////////////////////////////////////////////////////
// LocalRootSignature class methods
////////////////////////////////////////////////////////////////////////////////////////////

void LocalRootSignature::Init(const LocalRootSignatureDesc& desc) {
	// desc情報の保存
	rootSignature_ = desc.CreateRootSignature(Sxavenger::GetDevice());
	table_         = desc.GetTable();

	totalBufferSize_ = table_.size() * kGPUBufferSize;
}
