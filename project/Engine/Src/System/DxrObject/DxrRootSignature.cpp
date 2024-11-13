#include "DxrRootSignature.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GlobalRootSignatureDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

ComPtr<ID3D12RootSignature> GlobalRootSignatureDesc::CreateRootSignature(ID3D12Device* device) const {

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

	if (!params.empty()) {
		desc.pParameters = params.data();
		desc.NumParameters = static_cast<UINT>(params.size());
	}

	if (!samplers.empty()) {
		desc.pStaticSamplers = samplers.data();
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
		std::string log = "Create RootSignature failed. \n ";
		log += reinterpret_cast<char*>(signatureErrorBlob->GetBufferPointer());

		Assert(false, log);
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

////////////////////////////////////////////////////////////////////////////////////////////
// LocalRootSignatureDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void LocalRootSignatureDesc::Clear() {
	GlobalRootSignatureDesc::Clear();
	container_.Clear();
}

ComPtr<ID3D12RootSignature> LocalRootSignatureDesc::CreateRootSignature(ID3D12Device* device) const {

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

	if (!params.empty()) {
		desc.pParameters = params.data();
		desc.NumParameters = static_cast<UINT>(params.size());
	}

	if (!samplers.empty()) {
		desc.pStaticSamplers = samplers.data();
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
		std::string log = "Create RootSignature failed. \n ";
		log += reinterpret_cast<char*>(signatureErrorBlob->GetBufferPointer());

		Assert(false, log);
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
	container_.BindAddress(index);
}

void LocalRootSignatureDesc::SetVirtualSRV(uint32_t index, uint32_t shaderRegister) {
	GlobalRootSignatureDesc::SetVirtualSRV(index, shaderRegister);
	container_.BindAddress(index);
}

void LocalRootSignatureDesc::SetVirtualUAV(uint32_t index, uint32_t shaderRegister) {
	GlobalRootSignatureDesc::SetVirtualUAV(index, shaderRegister);
	container_.BindAddress(index);
}

void LocalRootSignatureDesc::SetSRV(uint32_t index, uint32_t shaderRegister) {
	GlobalRootSignatureDesc::SetSRV(index, shaderRegister);
	container_.BindHandle(index);
}

void LocalRootSignatureDesc::SetUAV(uint32_t index, uint32_t shaderRegister) {
	GlobalRootSignatureDesc::SetUAV(index, shaderRegister);
	container_.BindHandle(index);
}

void LocalRootSignatureDesc::Reserve() {
	GlobalRootSignatureDesc::Reserve();
}

////////////////////////////////////////////////////////////////////////////////////////////
// LocalRootSignature class
////////////////////////////////////////////////////////////////////////////////////////////

void LocalRootSignature::Create(const LocalRootSignatureDesc& desc) {
	rootSignature_   = desc.CreateRootSignature(Sxavenger::GetDevice());
	container_       = desc.GetContainer();
	bufferTotalSize_ = kGPUBufferSize * desc.params.size();
}

void LocalRootSignature::Term() {
}
