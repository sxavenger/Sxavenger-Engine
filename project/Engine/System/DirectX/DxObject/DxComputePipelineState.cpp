#include "DxComputePipelineState.h"
_DXOBJECT_USING

//=========================================================================================
// static variables
//=========================================================================================

CompileBlobCollection* ComputePipelineState::collection_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// ComputePipelineState class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ComputePipelineState::Term() {
}

void ComputePipelineState::CreateBlob(const std::wstring& filename) {
	Assert(collection_ != nullptr, "collection is not set.");
	blob_.blob     = collection_->TryCreateBlob(filename, CompileProfile::cs);
	blob_.filename = filename;
}

void ComputePipelineState::CreatePipeline(Device* device, const ComputeRootSignatureDesc& desc) {
	device_            = device;
	rootSignatureDesc_ = desc;

	CreateRootSignature();
	CreatePipeline();
}

void ComputePipelineState::ReloadShader() {
	Assert(collection_ != nullptr, "collection is not set.");
	
	if (blob_.blob.has_value()) {
		collection_->Reload(blob_.filename);
	}
	
	CheckAndUpdatePipeline();
}

void ComputePipelineState::CheckAndUpdatePipeline() {
	if (CheckShaderReloadStatus()) {
		CreatePipeline();
	}
}

void ComputePipelineState::SetPipeline(ID3D12GraphicsCommandList* commandList) const {
	commandList->SetPipelineState(pipeline_.Get());
	commandList->SetComputeRootSignature(rootSignature_.Get());
}

void ComputePipelineState::SetPipeline(CommandContext* context) const {
	SetPipeline(context->GetCommandList());
}

void ComputePipelineState::ReloadAndSetPipeline(ID3D12GraphicsCommandList* commandList) {
	CheckAndUpdatePipeline();
	SetPipeline(commandList);
}

void ComputePipelineState::ReloadAndSetPipeline(CommandContext* context) {
	ReloadAndSetPipeline(context->GetCommandList());
}

void ComputePipelineState::Dispatch(ID3D12GraphicsCommandList* commandList, UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ) const {
	commandList->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void ComputePipelineState::Dispatch(CommandContext* context, UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ) const {
	Dispatch(context->GetCommandList(), threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void ComputePipelineState::SetExternal(CompileBlobCollection* collection) {
	collection_ = collection;
}

void ComputePipelineState::CreateRootSignature() {
	Assert(device_ != nullptr, "device is not set.");
	rootSignature_ = rootSignatureDesc_.CreateComputeRootSignature(device_->GetDevice());
}

void ComputePipelineState::CreatePipeline() {
	Assert(device_ != nullptr, "device is not set.");

	D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
	desc.pRootSignature = rootSignature_.Get();
	desc.CS             = GetBytecode();

	auto hr = device_->GetDevice()->CreateComputePipelineState(
		&desc,
		IID_PPV_ARGS(&pipeline_)
	);
	Assert(SUCCEEDED(hr));
}

IDxcBlob* ComputePipelineState::GetBlob() {
	if (!blob_.blob.has_value()) {
		Assert(false, "blob is not set.");  //!< blobが設定されていない
		return {};
	}

	if (blob_.blob.value().expired()) {
		//!< hotReloadされている場合, 再度取得
		Assert(collection_ != nullptr, "external collection is not set."); //!< external collectionが設定されていない
		blob_.blob = collection_->GetBlob(blob_.filename);
	}

	auto blob = blob_.blob.value().lock();
	return (*blob).Get();
}

D3D12_SHADER_BYTECODE ComputePipelineState::GetBytecode() {
	IDxcBlob* blob = GetBlob();

	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.BytecodeLength  = blob->GetBufferSize();
	bytecode.pShaderBytecode = blob->GetBufferPointer();

	return bytecode;
}

bool ComputePipelineState::CheckShaderReloadStatus() {
	return blob_.blob.has_value() && blob_.blob.value().expired();
}

void ReflectionComputePipelineState::ReflectionRootSignature(Device* device) {
	device_ = device;

	table_.Reset();

	SetBlobToTable();

	rootSignatureDesc_ = table_.CreateComputeRootSignatureDesc();
	CreateRootSignature();
}

void ReflectionComputePipelineState::BindGraphicsBuffer(CommandContext* context, const BindBufferDesc& desc) {
	table_.BindGraphicsBuffer(context, desc);
}

void ReflectionComputePipelineState::CheckAndUpdatePipeline() {
	if (CheckShaderReloadStatus()) {
		ReflectionRootSignature(device_);
		CreatePipeline();
	}
}

void ReflectionComputePipelineState::SetBlobToTable() {
	if (!blob_.blob.has_value()) {
		Assert(false, "blob is not set."); //!< blobが設定されていない.
		return;
	}

	IDxcBlob* blob = GetBlob();

	ComPtr<ID3D12ShaderReflection> reflection = collection_->GetCompiler()->Reflection(blob);
	table_.CreateTable(reflection.Get(), ShaderVisibility::VISIBILITY_ALL);
}
