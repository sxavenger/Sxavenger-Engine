#include "DxComputePipelineState.h"
DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// ComputePipelineState class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ComputePipelineState::Term() {
}

void ComputePipelineState::SetBlob(const ShaderBlob& blob) {
	blob_ = blob;
}

void ComputePipelineState::CreateBlob(const std::filesystem::path& filepath) {
	std::unique_ptr<ShaderBlob> blob = std::make_unique<ShaderBlob>();
	blob->Create(filepath, CompileProfile::cs);

	SetBlob(*blob.get());
}

void ComputePipelineState::CreatePipeline(Device* device, ComputeRootSignatureDesc&& desc) {
	rootSignatureDesc_ = std::move(desc);
	
	CreateDirectXRootSignature(device);
	CreateDirectXPipeline(device);
}

void ComputePipelineState::SetPipeline(CommandContext* context) const {

	auto commandList = context->GetCommandList();

	commandList->SetPipelineState(pipeline_.Get());
	commandList->SetComputeRootSignature(rootSignature_.Get());
}

void ComputePipelineState::Dispatch(CommandContext* context, const Vector3ui& threadGroupCount) const {
	context->GetCommandList()->Dispatch(threadGroupCount.x, threadGroupCount.y, threadGroupCount.z);
}

D3D12_SHADER_BYTECODE ComputePipelineState::GetBytecode() {
	if (!blob_.has_value()) {
		Exception::Assert(false, "blob is not set.");  //!< blobが設定されていない
		return {};
	}

	return blob_.value().GetBytecode();
}

void ComputePipelineState::CreateDirectXRootSignature(Device* device) {
	rootSignature_ = rootSignatureDesc_.CreateComputeRootSignature(device->GetDevice());
}

void ComputePipelineState::CreateDirectXPipeline(Device* device) {
	D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
	desc.pRootSignature = rootSignature_.Get();
	desc.CS             = GetBytecode();

	auto hr = device->GetDevice()->CreateComputePipelineState(
		&desc,
		IID_PPV_ARGS(&pipeline_)
	);
	DxObject::Assert(hr, L"compute pipeline state create failed.");
}

////////////////////////////////////////////////////////////////////////////////////////////
// ReflectionComputePipelineState class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ReflectionComputePipelineState::ReflectionPipeline(Device* device) {
	table_.Reset();
	SetBlobToTable();

	rootSignatureDesc_ = table_.CreateComputeRootSignatureDesc();

	CreateDirectXRootSignature(device);
	CreateDirectXPipeline(device);
}

void ReflectionComputePipelineState::ReflectionPipeline(Device* device, const SamplerBindDesc& desc) {
	table_.Reset();
	SetBlobToTable();

	rootSignatureDesc_ = table_.CreateComputeRootSignatureDesc(desc);

	CreateDirectXRootSignature(device);
	CreateDirectXPipeline(device);
}


void ReflectionComputePipelineState::BindComputeBuffer(CommandContext* context, const BindBufferDesc& desc) {
	table_.BindComputeBuffer(context, desc);
}

void ReflectionComputePipelineState::SetBlobToTable() {
	if (!blob_.has_value()) {
		Exception::Assert(false, "blob is not set."); //!< blobが設定されていない.
		return;
	}

	ComPtr<ID3D12ShaderReflection> reflection = blob_.value().GetReflection();
	table_.CreateTable(reflection.Get(), ShaderVisibility::VISIBILITY_ALL);
}
