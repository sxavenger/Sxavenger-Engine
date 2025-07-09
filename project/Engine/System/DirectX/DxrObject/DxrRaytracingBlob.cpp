#include "DxrRaytracingBlob.h"
_DXROBJECT_USING

//=========================================================================================
// static variables
//=========================================================================================

DxObject::ShaderCompiler* RaytracingBlob::compiler_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RaytracingBlob::Create(const std::filesystem::path& filepath) {
	blob_ = compiler_->Compile(filepath, DxObject::CompileProfile::lib);
}

D3D12_SHADER_BYTECODE RaytracingBlob::GetBytecode() const {
	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.pShaderBytecode = blob_->GetBufferPointer();
	bytecode.BytecodeLength  = blob_->GetBufferSize();
	return bytecode;
}
