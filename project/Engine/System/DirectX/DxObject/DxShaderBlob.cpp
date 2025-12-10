#include "DxShaderBlob.h"
DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderBlob::Create(const std::filesystem::path& filepath, CompileProfile profile, const std::wstring& entrypoint) {
	// blobの生成
	blob_ = ShaderCompiler::GetInstance()->Compile(filepath, profile, entrypoint);
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
	return ShaderCompiler::GetInstance()->Reflection(blob_.Get());
}
