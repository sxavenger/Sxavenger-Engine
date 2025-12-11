#include "DxrRaytracingBlob.h"
DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RaytracingBlob::Create(const std::filesystem::path& filepath) {
	blob_ = DxObject::ShaderCompiler::GetInstance()->Compile(filepath, DxObject::CompileProfile::lib);
}

D3D12_SHADER_BYTECODE RaytracingBlob::GetBytecode() const {
	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.pShaderBytecode = blob_->GetBufferPointer();
	bytecode.BytecodeLength  = blob_->GetBufferSize();
	return bytecode;
}
