#include "DxShaderBlob.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

#include <Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlob methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::ShaderBlob::Init(const std::wstring& vsFilePath, const std::wstring& psFilePath) {
	// dxcCompilerの初期化
	{
		auto hr = DxcCreateInstance(
			CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_)
		);

		assert(SUCCEEDED(hr));
		Log("[DxObject.ShaderBlob]: dxcUtils_ << Complete Create \n");


		hr = DxcCreateInstance(
			CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_)
		);

		assert(SUCCEEDED(hr));
		Log("[DxObject.ShaderBlob]: dxcCompiler_ << Complete Create \n");
	}

	// includeHandleの初期化
	{
		auto hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);

		assert(SUCCEEDED(hr));
		Log("[DxObject.ShaderBlob]: includeHandler_ << Complete Create \n");
	}

	// shaderのコンパイル
	{
		shaderBlob_VS_ = DxObjectMethod::CompilerShader(
			vsFilePath, L"vs_6_0",
			dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get()
		);

		assert(shaderBlob_VS_ != nullptr);
		Log("[DxObject.ShaderBlob]: shaderBlob_VS_ << Complete Create \n");

		shaderBlob_PS_ = DxObjectMethod::CompilerShader(
			psFilePath, L"ps_6_0",
			dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get()
		);

		assert(shaderBlob_PS_ != nullptr);
		Log("[DxObject.ShaderBlob]: shaderBlob_PS_ << Complete Create \n");
	}
}

void DxObject::ShaderBlob::Term() {
}