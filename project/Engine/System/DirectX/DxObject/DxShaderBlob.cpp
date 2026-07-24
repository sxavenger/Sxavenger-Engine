#include "DxShaderBlob.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderBlob::Create(const std::filesystem::path& filepath, CompileProfile profile, const std::wstring& entry) {
	// blobの生成
	blob_ = ShaderCompiler::GetInstance()->Compile(filepath, profile, entry);
}

D3D12_SHADER_BYTECODE ShaderBlob::GetBytecode() const {
	StreamLogger::AssertA(blob_ != nullptr, "blob is null.");

	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.BytecodeLength  = blob_->GetBufferSize();
	bytecode.pShaderBytecode = blob_->GetBufferPointer();

	return bytecode;
}

ComPtr<ID3D12ShaderReflection> ShaderBlob::GetReflection() const {
	return ShaderCompiler::GetInstance()->Reflection(blob_.Get());
}
