#include "DxRootSignature.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <DxDevices.h>

#include <Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RootSignature methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::RootSignature::Init(Devices* devices, const RootSignatureDescs& descs) {

	// deviceの取り出し
	ID3D12Device* device = devices->GetDevice();

	// descの設定
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	if (!descs.param.empty()) {
		desc.pParameters   = descs.param.data();
		desc.NumParameters = static_cast<UINT>(descs.param.size());
	}

	if (!descs.sampler.empty()) {
		desc.pStaticSamplers   = descs.sampler.data();
		desc.NumStaticSamplers = static_cast<UINT>(descs.sampler.size());
	}

	auto hr = D3D12SerializeRootSignature(
		&desc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob_,
		&signatureErrorBlob_
	);

	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(signatureErrorBlob_->GetBufferPointer()));
		assert(false);
	}

	hr = device->CreateRootSignature(
		0,
		signatureBlob_->GetBufferPointer(),
		signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_)
	);

	assert(SUCCEEDED(hr));

	Log("[DxObject.RootSignature]: rootSignature_ << Complete Create \n");
}

void DxObject::RootSignature::Term() {
	signatureBlob_->Release();

	if (signatureErrorBlob_) {
		signatureErrorBlob_->Release();
	}
}
