#include "DxShaderBlob.h"
_DXOBJECT_USING

//=========================================================================================
// static variables
//=========================================================================================

ShaderCompiler* ShaderBlob::compiler_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderBlob::Create(const std::filesystem::path& filepath, CompileProfile profile, const std::wstring& entrypoint) {
	Assert(compiler_ != nullptr, "compiler is not set.");
	// blobの生成
	blob_ = compiler_->Compile(filepath, profile, entrypoint);
}

D3D12_SHADER_BYTECODE ShaderBlob::GetBytecode() const {
	if (blob_ != nullptr) {
		D3D12_SHADER_BYTECODE bytecode = {};
		bytecode.BytecodeLength  = blob_->GetBufferSize();
		bytecode.pShaderBytecode = blob_->GetBufferPointer();

		return bytecode;
	}

	return {};
}

ComPtr<ID3D12ShaderReflection> ShaderBlob::GetReflection() const {
	Assert(compiler_ != nullptr, "compiler is not set.");
	return compiler_->Reflection(blob_.Get());
}
