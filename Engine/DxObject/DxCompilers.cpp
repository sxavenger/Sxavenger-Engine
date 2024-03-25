#include "DxCompilers.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Logger.h>

// DxObject
#include <DxShaderBlob.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Compilers class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::Compilers::Init() {

	// dxcCompilerの初期化
	{
		auto hr = DxcCreateInstance(
			CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_)
		);

		assert(SUCCEEDED(hr));
		Log("[DxObject.Compilers]: dxcUtils_ << Complete Create \n");


		hr = DxcCreateInstance(
			CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_)
		);

		assert(SUCCEEDED(hr));
		Log("[DxObject.Compilers]: dxcCompiler_ << Complete Create \n");
	}

	// includeHandleの初期化
	{
		auto hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);

		assert(SUCCEEDED(hr));
		Log("[DxObject.Compilers]: includeHandler_ << Complete Create \n");
	}

	// shaderBlobに設定
	ShaderBlob::SetCompilders(this);

}

void DxObject::Compilers::Term() {
}
