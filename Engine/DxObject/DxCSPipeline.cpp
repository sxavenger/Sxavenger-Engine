#include "DxCSPipeline.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CSPipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::CSPipeline::CreatePipeline(const CSRootSignatureDesc& desc, CSBlob* csBlob) {

	rootSignature_ = desc.CreateRootSignature(Sxavenger::GetDevice());
	
	CreatePipeline(csBlob);
}

void DxObject::CSPipeline::SetPipeline() {

	// commandListの取り出し
	auto commandList = Sxavenger::GetCommandList();

	commandList->SetPipelineState(pipeline_.Get());
	commandList->SetComputeRootSignature(rootSignature_.Get());
}

void DxObject::CSPipeline::Dispatch(UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ) {
	// commandListの取り出し
	auto commandList = Sxavenger::GetCommandList();

	commandList->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void DxObject::CSPipeline::CreatePipeline(CSBlob* csBlob) {

	auto blob = csBlob->GetBlob();

	D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
	desc.pRootSignature = rootSignature_.Get();
	desc.CS             = { blob->GetBufferPointer(), blob->GetBufferSize() };

	auto hr = Sxavenger::GetDevice()->CreateComputePipelineState(
		&desc,
		IID_PPV_ARGS(&pipeline_)
	);

	Assert(SUCCEEDED(hr));

}
