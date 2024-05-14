#include "DxCompilers.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Logger.h>

// DxObject
#include <DxShaderBlob.h>
#include <DxShaderTable.h>

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
		Log("[DxObject.Compilers]: dxcUtils_ << Complete Create");


		hr = DxcCreateInstance(
			CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_)
		);

		assert(SUCCEEDED(hr));
		Log("[DxObject.Compilers]: dxcCompiler_ << Complete Create");
	}

	// includeHandleの初期化
	{
		auto hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);

		assert(SUCCEEDED(hr));
		Log("[DxObject.Compilers]: includeHandler_ << Complete Create");
	}

	// shaderBlobに設定
	ShaderTable::SetCompilers(this);

}

void DxObject::Compilers::Term() {
}
